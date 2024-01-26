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
#include <pb/ProgressManager.h>
#include <pb/config/Log.h>
#include <pb/export/ExportLogic.h>
#include <pb/export/Html.h>
#include <pb/export/Jpg.h>
#include <pb/export/Pdf.h>
#include <pb/persistence/Persistence.h>
#include <pb/persistence/ProjectPersistence.h>
#include <pb/project/Project.h>
#include <pb/tasks/FileMapper.h>
#include <pb/util/Util.h>

namespace PB {

class Photobook final
    : public ProjectPersistenceListener,
      public ImportFoldersLogicListener,
      public PBDev::ThreadScheduler,
      public PBDev::SequentialTaskConsumerListener<PdfExportTask>,
      public ProgressManagerListener {
public:
  explicit Photobook(Path localStatePath, Path installationPath);
  ~Photobook() = default;

  void configure(std::pair<int, int> screenSize);
  void configure(PhotobookListener *listener);
  void configure(StagedImagesListener *listener);
  void configure(ImageMonitorListener *listener);
  void configure(std::shared_ptr<Project> project);

  void recallMetadata();
  void recallProject(std::string name);

  void newProject(std::string name);
  void loadProject();
  void unloadProject();

  ImageViews         &imageViews();
  ProjectPersistence &project();

  void addImportFolder(Path importPath);
  void removeImportFolder(Path path);

  void loadStagedImages();

  void exportAlbum(std::string name, Path path);

  void onError(PBDev::Error error);

  void onProjectRead(
      std::vector<std::vector<std::shared_ptr<VirtualImage>>> &unstagedImages,
      std::vector<std::shared_ptr<VirtualImage>>              &stagedImages,
      std::vector<Path>                                       &roots) override;

  void onMetadataUpdated() override;

  void onPersistenceError(PBDev::Error) override;

  void onMappingStarted(Path path) override;
  void onMappingFinished(Path, std::vector<Path> newFolders) override;
  void onMappingAborted(Path) override;

  void onImportStop(Path) override;
  void onImageProcessed(Path key, Path root, Path full, Path medium,
                        Path small) override;

  void onImageProcessingJobEnded(Path root);

  void post(std::function<void()> f) override;

  void onProjectRenamed() override;

  void STCStarted(PdfExportTask const &task) override;
  void STCFinished(PdfExportTask const &task) override;
  void STCAborted(PdfExportTask const &task) override;
  void STCUpdate(PdfExportTask const &task) override;

  void progressUpdate(std::vector<std::string> names,
                      ProgressInfo             definedProgress,
                      ProgressInfo             undefinedProgress) override;

  std::vector<Path> pendingMappingPathList() const;

private:
  PhotobookListener             *mParent = nullptr;
  std::shared_ptr<PlatformInfo>  mPlatformInfo = nullptr;
  ProjectPersistence             mProjectPersistence;
  ImportFoldersLogic             mImportLogic;
  ImageViews                     mImageViews;
  CommandStack                   mCommandStack;
  bool                           mMarkProjectForDeletion = false;
  ExportLogic<PB::PdfExportTask> mExportLogic;
  ProgressManager                mProgressManager;
};
} // namespace PB