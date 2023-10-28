#pragma once

#include <string>

#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>

#include <thread_pool/thread_pool.h>

#include <pb/Command.h>
#include <pb/DataManager.h>
#include <pb/Error.h>
#include <pb/Exporter.h>
#include <pb/FileMapper.h>
#include <pb/Gallery.h>
#include <pb/ImageReader.h>
#include <pb/ImageSetWriter.h>
#include <pb/Pdf.h>
#include <pb/PhotobookListener.h>
#include <pb/Project.h>
#include <pb/ThumbnailsProcessor.h>
#include <pb/common/Log.h>
#include <pb/util/Concepts.h>
#include <pb/util/FileInfo.h>
#include <pb/util/Traits.h>
#include <pb/Html.h>
#include <pb/Jpg.h>

namespace PB {

class Photobook final : public Observer {
public:
  explicit Photobook(PhotobookListener &listener, Path centralPersistencePath,
            std::pair<int, int> screenSize);
  ~Photobook();

  void setPaperSettings(PaperSettings paperSettings);

  PaperSettings                      paperSettings();
  std::optional<PB::Path>            selectedImportFolder();
  Gallery                           &gallery();
  template <typename T> Exporter<T> &exporter();
  std::vector<Thumbnails>           &stagedPhotos();
  ImageSupport                      &imageSupport();

  void configureProject(Project project);
  void addImportFolder(Path importPath);
  void update(ObservableSubject &subject) override;
  void onImportFolderMapped(Path rootPath, std::vector<Path> newMediaMap);
  void onError(Error error);

  auto loadGalleryImage(std::string const &path, cv::Size size)
      -> std::shared_ptr<cv::Mat>;

  void discardPhotobook();
  void savePhotobook();
  void savePhotobook(Path newPath);
  void addStagedPhoto(std::vector<Thumbnails> photos, int position = -1);
  void deleteStagedPhoto(std::vector<int> positions);
  void removeStagedPhoto(int index);
  bool projectDefaultSaved();

private:
  PhotobookListener                    &mParent;
  Path                                  mCentralPersistencePath;
  SQLitePersistence                     mCentralPersistence;
  Project                               mProject;
  std::unordered_map<Path, MediaMapper> mMappingJobs;
  ImageSupport                          mImagePaths;
  Gallery                               mGallery;
  ImageReader                           mImageReader;
  ThumbnailsProcessor                   mThumbnailsProcessor;
  Exporter<Pdf>                         mExporter;
  std::unordered_map<Path, int>         mProgress;
  PaperSettings                         mPaperSettings;
  CommandStack                          mCommandStack;
};
} // namespace PB