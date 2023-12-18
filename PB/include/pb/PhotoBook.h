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
#include <pb/ImportFoldersLogic.h>
#include <pb/Jpg.h>
#include <pb/Pdf.h>
#include <pb/PhotobookListener.h>
#include <pb/Project.h>
#include <pb/SQLPersistence.h>
#include <pb/ThumbnailsProcessor.h>
#include <pb/common/Log.h>
#include <pb/persistence/Persistence.h>
#include <pb/util/Concepts.h>
#include <pb/util/FileInfo.h>
#include <pb/util/Traits.h>

namespace PB {

class Photobook final : public Observer,
                        public PersistenceMetadataListener,
                        public PersistenceProjectListener,
                        public ImportFoldersLogicListener,
                        public ThreadScheduler {
public:
  explicit Photobook(Path applicationLocalStatePath);
  ~Photobook() = default;

  void configure(std::pair<int, int> screenSize);
  void configure(std::shared_ptr<PhotobookListener> listener);
  void configure(StagedImagesListener *listener);
  void configure(ImageMonitorListener *listener);

  ImageViews &imageViews();

  void configureProject(Project project);

  void loadProject();

  void addImportFolder(Path importPath);

  void update(ObservableSubject &subject) override;
  void onError(Error error);

  void exportAlbum(std::string name, Path path);

  ProjectSnapshot &activeProject();

  void            discardPhotobook();
  void            savePhotobook();
  void            savePhotobook(Path newPath);
  ProjectSnapshot projectDetails();

  bool projectDefaultSaved();

  void onProjectRead(Project project) override;
  void onMetadataRead(ProjectMetadata projectMetadata) override;
  void onMetadataRead(std::vector<ProjectMetadata> projectMetadata) override;
  void onMetadataPersistenceError(Error) override;
  void onProjectPersistenceError(Error) override;

  std::shared_ptr<Persistence> persistence() { return mPersistence; }

  void newProject();

  void onMapped(Path, std::vector<Path> newFolders) override;
  void onImportStop(Path) override;
  void onImageProcessed(Path root, Path full, Path medium, Path small,
                        int progress, int progressCap) override;
  void onMappingAborted(Path) override;

  void post(std::function<void()> f) override;

private:
  void onImportFolderMapped(Path rootPath, std::vector<Path> newMediaMap);

  std::shared_ptr<PhotobookListener>       mParent = nullptr;
  std::shared_ptr<Persistence>             mPersistence;
  Path                                     mApplicationLocalStatePath;
  Project                                  mProject;
  ImportFoldersLogic                       mImportLogic;
  ImageViews                               mImageViews;
  ImageReader                              mImageReader;
  std::vector<std::shared_ptr<Exportable>> mExporters;
  std::unordered_map<Path, int>            mProgress;
  CommandStack                             mCommandStack;
  ExportFactory                            mExportFactory;
};
} // namespace PB