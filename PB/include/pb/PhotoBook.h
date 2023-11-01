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
#include <pb/Html.h>
#include <pb/ImageReader.h>
#include <pb/ImageSetWriter.h>
#include <pb/Jpg.h>
#include <pb/Pdf.h>
#include <pb/PhotobookListener.h>
#include <pb/Project.h>
#include <pb/ThumbnailsProcessor.h>
#include <pb/common/Log.h>
#include <pb/util/Concepts.h>
#include <pb/util/FileInfo.h>
#include <pb/util/Traits.h>

namespace PB {

class Photobook final : public Observer {
public:
  explicit Photobook(PhotobookListener &listener, Path centralPersistencePath,
                     std::pair<int, int> screenSize);
  ~Photobook();

  void setPaperSettings(PaperSettings paperSettings);

  PaperSettings paperSettings() const;

  Gallery      &gallery();
  ImageSupport &imageSupport();

  void configureProject(Project project);
  void newEmptyProject();

  void addImportFolder(Path importPath);

  void update(ObservableSubject &subject) override;
  void onError(Error error);

  void exportAlbum(std::string name, Path path);

  void discardPhotobook();
  void savePhotobook();
  void savePhotobook(Path newPath);

  bool projectDefaultSaved();

private:
  void onImportFolderMapped(Path rootPath, std::vector<Path> newMediaMap);

  PhotobookListener                       &mParent;
  Path                                     mCentralPersistencePath;
  SQLitePersistence                        mCentralPersistence;
  Project                                  mProject;
  std::unordered_map<Path, MediaMapper>    mMappingJobs;
  ImageSupport                             mImagePaths;
  Gallery                                  mGallery;
  ImageReader                              mImageReader;
  ThumbnailsProcessor                      mThumbnailsProcessor;
  std::vector<std::shared_ptr<Exportable>> mExporters;
  std::unordered_map<Path, int>            mProgress;
  PaperSettings                            mPaperSettings;
  CommandStack                             mCommandStack;
  ExportFactory                            mExportFactory;
};
} // namespace PB