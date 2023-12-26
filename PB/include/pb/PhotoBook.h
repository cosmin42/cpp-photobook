#pragma once

#include <string>

#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>

#include <thread_pool/thread_pool.h>

#include <pb/Command.h>
#include <pb/DataManager.h>
#include <pb/ImageSetWriter.h>
#include <pb/ImportFoldersLogic.h>
#include <pb/PhotobookListener.h>
#include <pb/config/Log.h>
#include <pb/export/Exporter.h>
#include <pb/export/Html.h>
#include <pb/export/Jpg.h>
#include <pb/export/Pdf.h>
#include <pb/persistence/Persistence.h>
#include <pb/project/Project.h>
#include <pb/tasks/FileMapper.h>
#include <pb/tasks/ThumbnailsProcessor.h>
#include <pb/util/Concepts.h>
#include <pb/util/Error.h>
#include <pb/util/FileInfo.h>
#include <pb/util/Traits.h>

namespace PB {

class Photobook final : public PBDev::Observer,
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
  void configure(DashboardListener *listener);
  void configure(Project project);

  void recallMetadata();
  void recallProject(Path path);

  void newProject();
  void deleteProject(std::string id);
  void discardProject();
  void saveProject();
  void saveProject(Path newPath);

  void loadProject();
  void unloadProject();

  ImageViews      &imageViews();
  ProjectSnapshot &activeProject();

  void addImportFolder(Path importPath);

  void loadStagedImages();

  void exportAlbum(std::string name, Path path);
  bool projectDefaultSaved();

  void onError(PBDev::Error error);

  void update(PBDev::ObservableSubject &subject) override;
  void onProjectRead(Project project) override;
  void onMetadataRead(ProjectMetadata projectMetadata) override;
  void onMetadataRead(std::vector<ProjectMetadata> projectMetadata) override;
  void onMetadataPersistenceError(PBDev::Error) override;
  void onProjectPersistenceError(PBDev::Error) override;

  void onMappingStarted(Path path) override;
  void onMappingFinished(Path, std::vector<Path> newFolders) override;
  void onImportStop(Path) override;
  void onImageProcessed(Path root, Path full, Path medium, Path small,
                        int progress, int progressCap) override;
  void onMappingAborted(Path) override;
  void post(std::function<void()> f) override;

private:

  std::shared_ptr<PhotobookListener>       mParent = nullptr;
  DashboardListener                       *mDashboardListener;
  Path                                     mApplicationLocalStatePath;
  Persistence                              mPersistence;
  std::shared_ptr<Project>                 mProject;
  ImportFoldersLogic                       mImportLogic;
  ImageViews                               mImageViews;
  std::vector<std::shared_ptr<Exportable>> mExporters;
  std::unordered_map<Path, int>            mProgress;
  CommandStack                             mCommandStack;
  ExportFactory                            mExportFactory;
};
} // namespace PB