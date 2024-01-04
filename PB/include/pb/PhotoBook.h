#pragma once

#include <string>

#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>

#include <pb/Command.h>
#include <pb/DataManager.h>
#include <pb/ImportFoldersLogic.h>
#include <pb/PhotobookListener.h>
#include <pb/Platform.h>
#include <pb/config/Log.h>
#include <pb/export/Exporter.h>
#include <pb/export/Html.h>
#include <pb/export/Jpg.h>
#include <pb/export/Pdf.h>
#include <pb/persistence/Persistence.h>
#include <pb/project/Project.h>
#include <pb/tasks/FileMapper.h>
#include <pb/util/Util.h>

namespace PB {

class Photobook final : public PBDev::Observer,
                        public PersistenceMetadataListener,
                        public PersistenceProjectListener,
                        public ImportFoldersLogicListener,
                        public ThreadScheduler {
public:
  explicit Photobook(Path localStatePath, Path installationPath);
  ~Photobook() = default;

  void configure(std::pair<int, int> screenSize);
  void configure(PhotobookListener *listener);
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
  void removeImportFolder(Path path);

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
  void onMappingAborted(Path) override;

  void onImportStop(Path) override;
  void onImageProcessed(Path root, Path full, Path medium, Path small) override;

  void post(std::function<void()> f) override;

  std::vector<Path> pendingMappingPathList() const;

private:
  PhotobookListener                       *mParent = nullptr;
  DashboardListener                       *mDashboardListener = nullptr;
  std::shared_ptr<PlatformInfo>            mPlatformInfo;
  Persistence                              mPersistence;
  std::shared_ptr<Project>                 mProject;
  ImportFoldersLogic                       mImportLogic;
  ImageViews                               mImageViews;
  std::vector<std::shared_ptr<Exportable>> mExporters;
  CommandStack                             mCommandStack;
  ExportFactory                            mExportFactory;
};
} // namespace PB