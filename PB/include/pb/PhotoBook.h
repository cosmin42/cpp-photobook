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
  PhotoBook(TaskManageableType &listener) : mParent(listener){};
  PhotoBook(PhotoBook const &) = delete;
  PhotoBook(PhotoBook &&other) = delete;
  PhotoBook &operator=(PhotoBook const &) = delete;
  ~PhotoBook() = default;

  void addMedia(std::string const &path)
  {
    PB::Path fsPath = path;
    auto     result = FileInfo::validInputRootPath(fsPath);
    std::visit(
        overloaded{
            [this](PB::Path const &path) {
              printDebug("Add media %s\n", path.string().c_str());
              mListeners.insert({path, MediaMapListener<TaskManageableType>(
                                           std::ref(*this))});
              auto listener = mListeners.at(path);
              mMappingJobs.insert(
                  {path, MediaMapper<TaskManageableType>(path, listener)});
            },
            [this](Error error) { mParent.onError(error); }},
        result);
    mMediaIndexedByPath.push_back(fsPath);
    mMappingJobs.at(fsPath).start();
  }

  void setOutputPath(std::string const &path)
  {
    PB::Path fsPath = path;
    auto     result = FileInfo::validOutputRootPath(fsPath);
    std::visit(overloaded{[this](PB::Path const &) mutable {},
                          [this](Error error) { mParent.doError(error); }},
               result);
  }

  auto rootPaths() const -> std::vector<Path> { return mMediaIndexedByPath; }

  auto mediaMap(unsigned index) -> std::optional<MediaMap>
  {
    assert(index < mMediaIndexedByPath.size());

    auto &key = mMediaIndexedByPath.at(index);

    assert(mMediaData.contains(key));

    return mMediaData.at(key);
  }

  std::optional<Path> getByIndex(unsigned index)
  {
    assert(index < mMediaIndexedByPath.size());
    return mMediaIndexedByPath.at(index);
  }

  void onNewMediaMap(Path &path, MediaMap &newMediaMap)
  {
    mMediaData.insert({path, newMediaMap});

    mMappingJobs.erase(path);
    mListeners.erase(path);
  }

private:
  auto loadImage(std::string const &path) -> std::optional<cv::Mat>
  {
    return ImageReader::defaultRead()(path);
  }

  // void exportImage([[maybe_unused]] std::string const &path) {}

  TaskManageableType                                            &mParent;
  std::unordered_map<Path, MediaMapListener<TaskManageableType>> mListeners;

  std::unordered_map<Path, MediaMapper<TaskManageableType>> mMappingJobs;
  std::unordered_map<Path, MediaMap>                        mMediaData;
  std::vector<Path>                                         mMediaIndexedByPath;
};
} // namespace PB