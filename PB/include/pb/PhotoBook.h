#pragma once

#include <string>

#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>

#include <pb/NoirListener.h>
#include <pb/PhotobookListener.h>
#include <pb/Platform.h>
#include <pb/components/OGLEngine.h>
#include <pb/components/Project.h>
#include <pb/components/VulkanManager.h>
#include <pb/export/ExportService.h>
#include <pb/export/Html.h>
#include <pb/export/Jpg.h>
#include <pb/export/Pdf.h>
#include <pb/image/ImageFactory.h>
#include <pb/infra/TaskCruncher.h>
#include <pb/jobs/PicturesSearchJob.h>
#include <pb/services/CollageService.h>
#include <pb/services/DatabaseService.h>
#include <pb/services/DirectoryInspectionService.h>
#include <pb/services/DurableHashService.h>
#include <pb/services/EffectsService.h>
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
                        public ProjectManagementServiceListener,
                        public EffectsServiceListener {
public:
  explicit Photobook(Path localStatePath, Path installationPath,
                     std::pair<unsigned, unsigned> screenSize);
  ~Photobook();

  void initLogger();

  void configure(PhotobookListener *listener);
  void configureNoirListener(NoirListener *listener);

  void startPhotobook();

  void makeCollages();

  void unloadProject();

  void removeImportFolder(Path path);

  std::shared_ptr<CollageService> collageService();

  std::shared_ptr<ImportFoldersService> importFoldersService();

  void onProjectRecalled() override;
  void onProjectMetadataRecalled(std::string focusedProjectName) override;

  void onMappingFinished(Path, std::vector<Path> newFolders) override;

  void onImageProcessed(Path key, Path root, GenericImagePtr image) override;

  void onImageProcessingJobEnded(Path root);

  void post(std::function<void()> f) override;

  void onExportComplete(Path) override;
  void onExportAborted(Path) override;
  void onExportUpdate(Path) override;

  void progressUpdate(PB::ProgressStatus status) override;

  void onThumbnailsCreated() override;
  void onCollageThumbnailsMakerError() override;

  void onCollageCreated(GenericImagePtr aggregatedImage) override;
  void onCollageMakerError() override;

  void onImportError(PBDev::Error error) override;

  void onImageMapped(PBDev::ImageToPaperId id, GenericImagePtr image) override;

  void onImageCopied(PBDev::ImageToPaperId imageId,
                     GenericImagePtr       image) override;

  [[deprecated]] void onLutAdded(LutIconInfo iconInfo) override;

  void onLutApplied(PBDev::LutApplicationId, GenericImagePtr) override;

  void onLutAppliedInMemory(PBDev::LutApplicationId,
                            std::shared_ptr<cv::Mat>) override;

  void onLutAppliedOnDiskInplace(PBDev::LutApplicationId) override;

  void onLutAppliedOnDisk(PBDev::LutApplicationId, GenericImagePtr) override;

  void onEffectApplied(PBDev::EffectId effectId,
                       GenericImagePtr image) override;
  void onEffectsApplicationError(PBDev::EffectId effectId,
                                 PB::ErrorCode) override;

  std::vector<Path> pendingMappingPathList() const;

  std::string projectName() const;

  std::shared_ptr<PlatformInfo> platformInfo() const;

  std::shared_ptr<ImageToPaperService> imageToPaperService() const;

  std::shared_ptr<ImageFactory> imageFactory() const;

  std::shared_ptr<ProjectManagementService> projectManagementService() const;

  std::string help(std::string name) const;

  std::shared_ptr<LutService> lutService() const;

  std::shared_ptr<ExportService> exportService() const;

private:
  PhotobookListener                        *mParent = nullptr;
  NoirListener                             *mNoirListener = nullptr;
  std::shared_ptr<TaskCruncher>             mTaskCruncher = nullptr;
  std::shared_ptr<PlatformInfo>             mPlatformInfo = nullptr;
  std::shared_ptr<DatabaseService>          mDatabaseService = nullptr;
  std::shared_ptr<ProjectSerializerService> mProjectSerializerService = nullptr;
  std::shared_ptr<DurableHashService>       mDurableHashService = nullptr;
  std::shared_ptr<ProjectManagementService> mProjectManagementService = nullptr;
  std::shared_ptr<ImageFactory>             mImageFactory = nullptr;

  std::shared_ptr<ImportFoldersService> mImportLogic = nullptr;
  bool                                  mMarkProjectForDeletion = false;
  std::shared_ptr<ExportService>        mExportService = nullptr;
  std::shared_ptr<ProgressService>      mProgressService = nullptr;
  std::shared_ptr<ImageToPaperService>  mImageToPaperService = nullptr;
  std::shared_ptr<CollageService>       mCollageTemplateManager = nullptr;
  std::shared_ptr<LutService>           mLutService = nullptr;
  std::shared_ptr<VulkanManager>        mVulkanManager = nullptr;
  std::shared_ptr<OGLEngine>            mOGLEngine = nullptr;
  std::shared_ptr<EffectsService>       mEffectsService = nullptr;
};
} // namespace PB