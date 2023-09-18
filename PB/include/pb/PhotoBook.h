#pragma once

#include <string>

#include <pb/DataManager.h>
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
  PhotoBook(TaskManageableType &listener)
      : mParent(listener), mGalleryListener(std::ref(*this)),
        mGallery(mGalleryListener)
  {
  }
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

                     auto ptr =
                         std::make_shared<MediaMapListener<TaskManageableType>>(
                             std::ref(*this));
                     mListeners.insert({path, ptr});
                     auto listener = mListeners.at(path);
                     mMappingJobs.emplace(
                         path, MediaMapper<TaskManageableType>(path, listener));
                   },
                   [this](Error error) { mParent.onError(error); }},
        result);
    Context::inst().data().mediaIndexedByType().push_back(fsPath);
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

  auto mediaMap(unsigned index) -> std::optional<MediaMap>
  {
    auto &mediaIndexedByType = Context::inst().data().mediaIndexedByType();
    assert(index < mediaIndexedByType.size());

    auto &key = mediaIndexedByType.at(index);

    auto &mediaData = Context::inst().data().mediaData();
    assert(mediaData.contains(key));

    return mediaData.at(key);
  }

  void onNewMediaMap(Path &path, MediaMap &newMediaMap)
  {
    auto &mediaData = Context::inst().data().mediaData();
    mediaData.insert({path, newMediaMap});

    mParent.onFinished();
    // mMappingJobs.erase(path);
    // mListeners.erase(path);
  }

  Gallery<TaskManageableType> &gallery() { return mGallery; }

  auto loadImage(std::string const &path) -> std::shared_ptr<cv::Mat>
  {
    return mImageReader.read(path);
  }

  auto loadGalleryImage(std::string const &path, int32_t width,
                     int32_t height) -> std::shared_ptr<cv::Mat>
  {
    return ImageReader::resize(mImageReader.read(path), width, height);
  }

private:
  // void exportImage([[maybe_unused]] std::string const &path) {}

  TaskManageableType &mParent;
  std::unordered_map<Path,
                     std::shared_ptr<MediaMapListener<TaskManageableType>>>
      mListeners;

  std::unordered_map<Path, MediaMapper<TaskManageableType>> mMappingJobs;

  GalleryListener<TaskManageableType> mGalleryListener;
  Gallery<TaskManageableType>         mGallery;

  ImageReader mImageReader;
};
} // namespace PB