#pragma once

#include <string>

#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>

#include <pb/DataManager.h>
#include <pb/Error.h>
#include <pb/FileMapper.h>
#include <pb/Gallery.h>
#include <pb/ImageReader.h>
#include <pb/Settings.h>
#include <pb/StorageListener.h>
#include <pb/common/Log.h>
#include <pb/util/Concepts.h>
#include <pb/util/FileInfo.h>
#include <pb/util/Traits.h>

namespace PB {

template <typename PhotoBookType, typename PersistenceType>
  requires PhotoBookConcept<PhotoBookType>
class PhotoBook final {
public:
  PhotoBook(Settings const settings, PhotoBookType &listener)
      : mSettings(settings), mParent(listener),
        mStorageListener(std::ref(*this)), mGalleryListener(std::ref(*this)),
        mGallery(mGalleryListener)
  {
    printDebug("Photobook created. %s\n", settings.projectFolder.c_str());

    mPersistence.load([this](std::optional<Error> maybeError) {
      if (maybeError) {
        onError(maybeError.value());
      }
      else {
        boost::uuids::uuid newUUID = boost::uuids::random_generator()();
        mPersistence.cache()[boost::uuids::to_string(newUUID)] = "project-name";
        mPersistence.write([](std::optional<Error>) {});
      }
    });
  }
  PhotoBook(PhotoBook const &) = delete;
  PhotoBook(PhotoBook &&other) = delete;
  PhotoBook &operator=(PhotoBook const &) = delete;
  ~PhotoBook()
  {
    printDebug("Photobook destructed.\n");
    Context::inst().data().clear();
  }

  void onPersistenceLoaded() { printDebug("Persistence loaded.\n"); }

  void onError(Error error) { mParent.onError(error); }

  void addMedia(std::string const &path)
  {
    PB::Path fsPath = path;
    auto     result = FileInfo::validInputRootPath(fsPath);
    std::visit(
        overloaded{[this](PB::Path const &path) {
                     printDebug("Add media %s\n", path.string().c_str());

                     auto ptr = std::make_shared<
                         MediaMapListener<PhotoBookType, PersistenceType>>(
                         std::ref(*this));
                     mListeners.insert({path, ptr});
                     auto listener = mListeners.at(path);
                     mMappingJobs.emplace(
                         path, MediaMapper<PhotoBookType, PersistenceType>(
                                   path, listener));
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

  Gallery<PhotoBookType, PersistenceType> &gallery() { return mGallery; }

  auto loadImage(std::string const &path) -> std::shared_ptr<cv::Mat>
  {
    return mImageReader.read(path);
  }

  auto loadGalleryImage(std::string const &path, cv::Size size)
      -> std::shared_ptr<cv::Mat>
  {
    auto image = mImageReader.read(path);
    return Process::resize(size, true)(image);
  }

  void exportAlbum([[maybe_unused]] std::string const &path)
  {
    PB::printDebug("Export image to %s", path.c_str());
  }

  void discardPhotoBook() { PB::printDebug("Discard Photobook\n"); }

  void savePhotoBook() { PB::printDebug("Save Photobook\n"); }

private:
  std::string mProjectName;

  Settings mSettings;

  PhotoBookType &mParent;

  StorageListener<PhotoBookType, PersistenceType> mStorageListener;
  Persistence<PersistenceType>                    mPersistence;

  std::unordered_map<
      Path, std::shared_ptr<MediaMapListener<PhotoBookType, PersistenceType>>>
      mListeners;

  std::unordered_map<Path, MediaMapper<PhotoBookType, PersistenceType>>
      mMappingJobs;

  GalleryListener<PhotoBookType, PersistenceType> mGalleryListener;
  Gallery<PhotoBookType, PersistenceType>         mGallery;

  ImageReader mImageReader;
};
} // namespace PB