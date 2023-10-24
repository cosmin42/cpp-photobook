#pragma once

#include <string>

#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>

#include <thread_pool/thread_pool.h>

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

class PhotoBook final : public Observer {
public:
  PhotoBook(PhotobookListener &listener, Path centralPersistencePath,
            std::pair<int, int> screenSize);
  PhotoBook(PhotoBook const &) = delete;
  PhotoBook(PhotoBook &&other) = delete;
  PhotoBook &operator=(PhotoBook const &) = delete;
  ~PhotoBook();

  PaperSettings paperSettings();
  void          setPaperSettings(PaperSettings paperSettings);
  void          loadProject(Path const                               &path,
                            std::function<void(std::optional<Error>)> onReturn);
  void          addImportFolder(Path importPath);
  void          update(ObservableSubject &subject) override;
  void     onImportFolderMapped(Path rootPath, std::vector<Path> newMediaMap);
  void     onError(Error error);
  Gallery &gallery();
  std::optional<PB::Path> selectedImportFolder();
  auto loadGalleryImage(std::string const &path, cv::Size size)
      -> std::shared_ptr<cv::Mat>;
  template <typename T> Exporter<T> &exporter();
  void                               discardPhotoBook();
  void                               savePhotoBook();
  void                               savePhotoBook(Path newPath);
  void                               addStagedPhoto(Thumbnails th);
  void                     deleteStagedPhoto(std::vector<int> positions);
  void                     insertStagedPhoto(Thumbnails path, int position);
  void                     removeStagedPhoto(int index);
  std::vector<Thumbnails> &stagedPhotos();
  ImageSupport            &imageSupport();
  bool                     projectDefaultSaved();

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
};
} // namespace PB