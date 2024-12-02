#pragma once

#include <string>

#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>

#include <pb/PhotobookListener.h>
#include <pb/Platform.h>
#include <pb/components/OGLEngine.h>
#include <pb/components/TaskCruncher.h>
#include <pb/export/ExportLogic.h>
#include <pb/export/Html.h>
#include <pb/export/Jpg.h>
#include <pb/export/Pdf.h>
#include <pb/image/ImageFactory.h>
#include <pb/jobs/PicturesSearchJob.h>
#include <pb/project/Project.h>
#include <pb/services/CollageService.h>
#include <pb/services/DatabaseService.h>
#include <pb/services/DirectoryInspectionService.h>
#include <pb/services/DurableHashService.h>
#include <pb/services/ImageToPaperService.h>
#include <pb/services/ImportFoldersService.h>
#include <pb/services/LutService.h>
#include <pb/services/ProgressService.h>
#include <pb/services/ProjectManagementService.h>
#include <pb/services/ProjectSerializerService.h>

namespace PB {

class Photobook final : public ImportFoldersServiceListener,
                        public PBDev::ThreadScheduler,
                        public ProgressServiceListener,
                        public ExportListener,
                        public CollageThumbnailsMakerListener,
                        public ImageToPaperServiceListener,
                        public CollageMakerListener,
                        public LutServiceListener,
                        public ProjectManagementServiceListener {
public:
  explicit Photobook(Path localStatePath, Path installationPath,
                     std::pair<unsigned, unsigned> screenSize);
  ~Photobook();

  void initLogger();

  void configure(PhotobookListener *listener);

  void startPhotobook();

  void recallMetadata();
  void recallProject(std::string name);

  void newProject(std::string name, PaperSettings paperSettings);
  void unloadProject();

  void addImportFolder(Path importPath);
  void removeImportFolder(Path path);

  void exportPDFAlbum(std::string name, Path path);
  void exportPDFLibharu(std::string name, Path path);
  void exportJPGAlbum(std::string name, Path path);

  void stopProjectWork();

  bool isProjectWorking() const;

  std::shared_ptr<CollageService> collageService();

  /*
  void onProjectRead(
      std::vector<std::vector<std::shared_ptr<VirtualImage>>> &unstagedImages,
      std::vector<std::shared_ptr<VirtualImage>>              &stagedImages,
      std::vector<Path>                                       &roots) override;

  void onMetadataUpdated() override;

  void onPersistenceError(PBDev::Error) override;
  */

  void onProjectRecalled() override;
  void onProjectMetadataRecalled() override;

  void onMappingFinished(Path, std::vector<Path> newFolders) override;

  void onImageProcessed(Path key, Path root, GenericImagePtr image) override;

  void onImageProcessingJobEnded(Path root);

  void post(std::function<void()> f) override;

  // void onProjectRenamed() override;

  void onExportComplete(std::string name) override;
  void onExportAborted(std::string name) override;
  void onExportUpdate(std::string name) override;

  void progressUpdate(PB::ProgressStatus status) override;

  void onThumbnailsCreated() override;
  void onCollageThumbnailsMakerError() override;

  void onCollageCreated(unsigned index, Path imagePath) override;
  void onCollageMakerError() override;

  void onImageMapped(PBDev::ImageToPaperId id, GenericImagePtr image) override;

  void onLutAdded(LutIconInfo iconInfo) override;

  std::vector<Path> pendingMappingPathList() const;

  std::string projectName() const;

  std::shared_ptr<PlatformInfo> platformInfo() const;

  std::shared_ptr<ImageToPaperService> imageToPaperService() const;

  std::shared_ptr<ImageFactory> imageFactory() const;

  std::shared_ptr<ProjectManagementService> projectManagementService() const;

private:
  PhotobookListener                        *mParent = nullptr;
  std::shared_ptr<TaskCruncher>             mTaskCruncher = nullptr;
  std::shared_ptr<PlatformInfo>             mPlatformInfo = nullptr;
  std::shared_ptr<DatabaseService>          mDatabaseService = nullptr;
  std::shared_ptr<ProjectSerializerService> mProjectSerializerService = nullptr;
  std::shared_ptr<DurableHashService>       mDurableHashService = nullptr;
  std::shared_ptr<ProjectManagementService> mProjectManagementService = nullptr;
  std::shared_ptr<ImageFactory>             mImageFactory = nullptr;

  std::shared_ptr<ImportFoldersService>       mImportLogic = nullptr;
  bool                                        mMarkProjectForDeletion = false;
  ExportLogic                                 mExportLogic;
  std::shared_ptr<ProgressService>            mProgressService = nullptr;
  std::shared_ptr<ImageToPaperService>        mImageToPaperService = nullptr;
  std::shared_ptr<CollageService>             mCollageTemplateManager = nullptr;
  std::shared_ptr<LutService>                 mLutService = nullptr;
  std::shared_ptr<OGLEngine> mOGLEngine = nullptr;
};
} // namespace PB