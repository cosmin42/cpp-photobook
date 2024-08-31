#pragma once

#include <string>

#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>

#include <pb/CollageManager.h>
#include <pb/Command.h>
#include <pb/DataManager.h>
#include <pb/DirectoryInspectionService.h>
#include <pb/ImageToPaperService.h>
#include <pb/ImportFoldersLogic.h>
#include <pb/LutService.h>
#include <pb/OGLEngine.h>
#include <pb/PhotobookListener.h>
#include <pb/Platform.h>
#include <pb/ProgressManager.h>
#include <pb/TaskCruncher.h>
#include <pb/export/ExportLogic.h>
#include <pb/export/Html.h>
#include <pb/export/Jpg.h>
#include <pb/export/Pdf.h>
#include <pb/persistence/Persistence.h>
#include <pb/persistence/PersistenceService.h>
#include <pb/project/Project.h>
#include <pb/tasks/PicturesSearchConfig.h>
#include <pb/util/Util.h>

namespace PB {

class Photobook final : public PersistenceServiceListener,
                        public ImportFoldersLogicListener,
                        public PBDev::ThreadScheduler,
                        public ProgressManagerListener,
                        public ExportListener,
                        public CollageThumbnailsMakerListener,
                        public ImageToPaperServiceListener,
                        public CollageMakerListener {
public:
  explicit Photobook(Path localStatePath, Path installationPath,
                     std::pair<unsigned, unsigned> screenSize);
  ~Photobook() = default;

  void configure(PhotobookListener *listener);
  void configure(StagedImagesListener *listener);
  void configure(ImageMonitorListener *listener);
  void configure(std::shared_ptr<Project> project);

  void configureCurrentProject();

  void startPhotobook();

  void recallMetadata();
  void recallProject(std::string name);

  void newProject(std::string name, PaperSettings paperSettings);
  void unloadProject();

  ImageViews                         &imageViews();
  std::shared_ptr<PersistenceService> project();

  void addImportFolder(Path importPath);
  void removeImportFolder(Path path);

  void exportPDFAlbum(std::string name, Path path);
  void exportPDFLibharu(std::string name, Path path);
  void exportJPGAlbum(std::string name, Path path);

  std::shared_ptr<CollageManager> collageManager();

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
  void onImageProcessed(Path key, Path root,
                        ImageResources imageResources) override;

  void onImageProcessingJobEnded(Path root);

  void post(std::function<void()> f) override;

  void onProjectRenamed() override;

  void onExportComplete(std::string name) override;
  void onExportAborted(std::string name) override;
  void onExportUpdate(std::string name) override;

  void progressUpdate(PB::ProgressStatus status) override;

  void onThumbnailsCreated() override;
  void onCollageThumbnailsMakerError() override;

  void onCollageCreated(unsigned index, Path imagePath) override;
  void onCollageMakerError() override;

  void onImageMapped(PBDev::ImageToPaperId         id,
                     std::shared_ptr<VirtualImage> image) override;

  std::vector<Path> pendingMappingPathList() const;

  std::string projectName() const;

  std::shared_ptr<PlatformInfo> platformInfo() const;

  std::shared_ptr<ImageToPaperService> imageToPaperService() const;

private:
  PhotobookListener                          *mParent = nullptr;
  std::shared_ptr<TaskCruncher>               mTaskCruncher = nullptr;
  std::shared_ptr<PlatformInfo>               mPlatformInfo = nullptr;
  std::shared_ptr<PersistenceService>         mPersistenceService = nullptr;
  ImportFoldersLogic                          mImportLogic;
  ImageViews                                  mImageViews;
  CommandStack                                mCommandStack;
  bool                                        mMarkProjectForDeletion = false;
  ExportLogic                                 mExportLogic;
  std::shared_ptr<ProgressManager>            mProgressManager;
  std::string                                 mProjectName;
  std::shared_ptr<ImageToPaperService>        mImageToPaperService = nullptr;
  std::shared_ptr<CollageManager>             mCollageTemplateManager = nullptr;
  std::shared_ptr<LutService>                 mLutService = nullptr;
  std::shared_ptr<DirectoryInspectionService> mDirectoryInspectionService =
      nullptr;
  std::shared_ptr<OGLEngine>                  mOGLEngine = nullptr;
};
} // namespace PB