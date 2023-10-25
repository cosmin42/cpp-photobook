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

namespace PB {

enum class PaperType { None, A4, A5, A3, Custom };

struct PaperSettings {
  PaperType type;
  int       ppi;
  int       width;
  int       height;
};

static constexpr PaperSettings A4_PAPER = {PaperType::A4, 300, 3508, 2480};
static constexpr PaperSettings A5_PAPER = {PaperType::A5, 300, 2480, 1748};
static constexpr PaperSettings A3_PAPER = {PaperType::A3, 300, 4961, 3508};

class Photobook final : public Observer {
public:
  Photobook(PhotobookListener &listener, Path centralPersistencePath,
            std::pair<int, int> screenSize);
  Photobook(Photobook const &) = delete;
  Photobook(Photobook &&other) = delete;
  Photobook &operator=(Photobook const &) = delete;
  Photobook &operator=(Photobook &&) = delete;
  ~Photobook();

  void setPaperSettings(PaperSettings paperSettings);

  PaperSettings                      paperSettings();
  std::optional<PB::Path>            selectedImportFolder();
  Gallery                           &gallery();
  template <typename T> Exporter<T> &exporter();
  std::vector<Thumbnails>           &stagedPhotos();
  ImageSupport                      &imageSupport();

  void loadProject(Path const                               &path,
                   std::function<void(std::optional<Error>)> onReturn);
  void addImportFolder(Path importPath);
  void update(ObservableSubject &subject) override;
  void onImportFolderMapped(Path rootPath, std::vector<Path> newMediaMap);
  void onError(Error error);

  auto loadGalleryImage(std::string const &path, cv::Size size)
      -> std::shared_ptr<cv::Mat>;

  void discardPhotobook();
  void savePhotobook();
  void savePhotobook(Path newPath);
  void addStagedPhoto(std::vector<Thumbnails> photos);
  void deleteStagedPhoto(std::vector<int> positions);
  void insertStagedPhoto(std::vector<Thumbnails> path, int position);
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