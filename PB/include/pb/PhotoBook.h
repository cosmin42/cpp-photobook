#pragma once

#include <string>

#include <pb/Error.h>
#include <pb/FileMapper.h>
#include <pb/Gallery.h>
#include <pb/ImageReader.h>
#include <pb/common/Log.h>
#include <pb/util/Concepts.h>
#include <pb/util/FileInfo.h>
#include <pb/util/Traits.h>

namespace PB {

template <typename TaskManageableType>
  requires TaskManageableConcept<TaskManageableType>
class PhotoBook final {
public:
  PhotoBook(TaskManageableType &listener) : mListener(listener){};
  PhotoBook(PhotoBook const &) = delete;
  PhotoBook(PhotoBook &&other) = delete;
  PhotoBook &operator=(PhotoBook const &) = delete;
  ~PhotoBook() = default;

  void addMedia(std::string const &path)
  {
    PB::Path fsPath = path;
    auto     result = FileInfo::validInputRootPath(fsPath);
    std::visit(
        overloaded{[this](PB::Path const &path) {
                     printDebug("Add media %s\n", path.string().c_str());
                     auto mapperPtr =
                         std::make_shared<MediaMapper<TaskManageableType>>(
                             path, mListener);
                     mMediaFolders.insert({path, mapperPtr});
                   },
                   [this](Error error) { mListener.onError(error); }},
        result);
    mMediaFolderPaths.push_back(fsPath);
    mMediaFolders.at(fsPath)->start();
  }

  void setOutputPath(std::string const &path)
  {
    PB::Path fsPath = path;
    auto     result = FileInfo::validOutputRootPath(fsPath);
    std::visit(
        overloaded{[this](PB::Path const &path) mutable { mOutputPath = path; },
                   [this](Error error) { mListener.doError(error); }},
        result);
  }

  auto rootPaths() const -> std::vector<Path> { return mMediaFolderPaths; }

  auto mediaMap(unsigned index) -> std::optional<MediaMap>
  {
    assert(index < mMediaFolderPaths.size());

    auto &key = mMediaFolderPaths.at(index);

    assert(mMediaData.contains(key));

    return mMediaData.at(key);
  }

  std::optional<Path> getByIndex(unsigned index)
  {
    assert(index < mMediaFolderPaths.size());
    return mMediaFolderPaths.at(index);
  }

private:
  auto loadImage(std::string const &path) -> std::optional<cv::Mat>
  {
    return ImageReader::defaultRead()(path);
  }

  void exportImage([[maybe_unused]] std::string const &path) {}

  TaskManageableType &mListener;

  std::unordered_map<Path, std::shared_ptr<MediaMapper<TaskManageableType>>>
                                     mMediaFolders;
  std::unordered_map<Path, MediaMap> mMediaData;
  std::vector<Path>                  mMediaFolderPaths;
  std::optional<Path>                mOutputPath = std::nullopt;
  std::vector<std::filesystem::path> mImagesMapCache;
};
} // namespace PB