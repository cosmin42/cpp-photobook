#include <pb/PhotoBook.h>

#include <pb/Config.h>
#include <pb/components/ImportImageTask.h>
#include <pb/export/PdfLibharu.h>
#include <pb/export/PdfPoDoFo.h>
#include <pb/image/ImageFactory.h>

using namespace PB::Service;

namespace PB {

// TODO: Break this into multiple methods
Photobook::Photobook(Path localStatePath, Path installationPath,
                     std::pair<unsigned, unsigned> screenSize)
    : mTaskCruncher(std::make_shared<TaskCruncher>()),
      mPlatformInfo(std::make_shared<PlatformInfo>(installationPath,
                                                   localStatePath, screenSize)),
      mDatabaseService(std::make_shared<DatabaseService>()),
      mProjectSerializerService(std::make_shared<ProjectSerializerService>()),
      mDurableHashService(std::make_shared<DurableHashService>()),
      mProjectManagementService(std::make_shared<ProjectManagementService>()),
      mImageFactory(std::make_shared<ImageFactory>()),
      mImportLogic(std::make_shared<ImportFoldersService>()),
      mExportService(std::make_shared<ExportService>()),
      mProgressService(std::make_shared<ProgressService>()),
      mImageToPaperService(std::make_shared<ImageToPaperService>()),
      mCollageTemplateManager(std::make_shared<CollageService>()),
      mLutService(std::make_shared<LutService>()),
      mVulkanManager(std::make_shared<VulkanManager>()),
      mOGLEngine(std::make_shared<OGLEngine>()),
      mEffectsService(std::make_shared<EffectsService>())
{

  initLogger();

  auto importFoldersServiceListener =
      dynamic_cast<ImportFoldersServiceListener *>(this);
  PBDev::basicAssert(importFoldersServiceListener != nullptr);

  auto threadScheduler = dynamic_cast<PBDev::ThreadScheduler *>(this);
  PBDev::basicAssert(threadScheduler != nullptr);

  auto progressServiceListener = dynamic_cast<ProgressServiceListener *>(this);
  PBDev::basicAssert(progressServiceListener != nullptr);
  mProgressService->configure(progressServiceListener);

  auto effectsServiceListener = dynamic_cast<EffectsServiceListener *>(this);
  PBDev::basicAssert(effectsServiceListener != nullptr);
  mEffectsService->configureEffectsServiceListener(effectsServiceListener);

  auto imageToPaperServiceListener =
      dynamic_cast<PB::ImageToPaperServiceListener *>(this);
  PBDev::basicAssert(imageToPaperServiceListener != nullptr);
  mImageToPaperService->setImageToPaperServiceListener(
      imageToPaperServiceListener);

  mDatabaseService->configurePlatformInfo(mPlatformInfo);

  mDurableHashService->configureDatabaseService(mDatabaseService);

  auto lutServiceListener = dynamic_cast<LutServiceListener *>(this);
  PBDev::basicAssert(lutServiceListener != nullptr);
  mLutService->configureLutServiceListener(lutServiceListener);

  auto projectManagementServiceListener =
      dynamic_cast<ProjectManagementServiceListener *>(this);
  PBDev::basicAssert(projectManagementServiceListener != nullptr);
  mProjectManagementService->configureProjectManagementServiceListener(
      projectManagementServiceListener);

  mOGLEngine->configurePlatformInfo(mPlatformInfo);
  mOGLEngine->configVulkanManager(mVulkanManager);

  mProgressService->configureScheduler(threadScheduler);

  mTaskCruncher->configureProgressService(mProgressService);

  auto exportListener = dynamic_cast<PB::ExportListener *>(this);
  PBDev::basicAssert(exportListener != nullptr);
  mExportService->configureExportListener(exportListener);

  mProjectManagementService->configurePlatformInfo(mPlatformInfo);
  mProjectManagementService->configureProjectSerializerService(
      mProjectSerializerService);

  mProjectSerializerService->configurePlatformInfo(mPlatformInfo);

  mImportLogic->configureTaskCruncher(mTaskCruncher);
  mImportLogic->configurePlatformInfo(mPlatformInfo);
  mImportLogic->configureListener(importFoldersServiceListener);

  mExportService->configureTaskCruncher(mTaskCruncher);
  mExportService->configurePlatformInfo(mPlatformInfo);

  mCollageTemplateManager->configureTaskCruncher(mTaskCruncher);

  mImageFactory->configurePlatformInfo(mPlatformInfo);
  mImageFactory->configureDurableHashService(mDurableHashService);

  mImageToPaperService->configurePlatformInfo(mPlatformInfo);
  mImageToPaperService->configureTaskCruncher(mTaskCruncher);
  mImageToPaperService->configureImageFactory(mImageFactory);

  mLutService->configurePlatformInfo(mPlatformInfo);
  mLutService->configureTaskCruncher(mTaskCruncher);
  mLutService->configureThreadScheduler(threadScheduler);
  mLutService->configureOGLEngine(mOGLEngine);
  mLutService->configureDurableHashService(mDurableHashService);
  mLutService->configureImageFactory(mImageFactory);

  mCollageTemplateManager->configurePlatformInfo(mPlatformInfo);
  mCollageTemplateManager->configureVulkanManager(mVulkanManager);

  auto collageThumbnailsMakerListener =
      dynamic_cast<CollageThumbnailsMakerListener *>(this);
  PBDev::basicAssert(collageThumbnailsMakerListener != nullptr);
  mCollageTemplateManager->configureThumbnailsListener(
      collageThumbnailsMakerListener);

  auto collageMakerListener = dynamic_cast<CollageMakerListener *>(this);
  PBDev::basicAssert(collageMakerListener != nullptr);
  mCollageTemplateManager->configureCollageMakerListener(collageMakerListener);

  mProjectManagementService->configureDatabaseService(mDatabaseService);

  mEffectsService->configurePlatformInfo(mPlatformInfo);
  mEffectsService->configureImageFactory(mImageFactory);
  mEffectsService->configureTaskCruncher(mTaskCruncher);

  for (auto const &[poolName, poolSize] : OneConfig::TASK_CRUNCHER_POOLS_INFO) {
    mTaskCruncher->registerPTC(poolName, poolSize);
  }
}

Photobook::~Photobook() { mDatabaseService->disconnect(); }

void Photobook::initLogger() { std::ignore = Noir::inst().getLogger(); }

void Photobook::configure(PhotobookListener *listener) { mParent = listener; }

void Photobook::configureNoirListener(NoirListener *listener)
{
  mNoirListener = listener;
}

void Photobook::startPhotobook()
{

  // TODO: Why is this method caled multiple times?
  static bool once = false;
  if (!once) {
    mVulkanManager->init();
    mDatabaseService->connect();
    mDatabaseService->maybeCreateTables();
    mOGLEngine->start();
    mLutService->startLutService();
    once = true;
  }
}

void Photobook::unloadProject()
{
  auto maybeProject = mProjectManagementService->maybeLoadedProjectInfo();
  if (maybeProject) {
    mCollageTemplateManager->stop();
    // TODO: Improve this
    while (mCollageTemplateManager->isRunning())
      ;

    mCollageTemplateManager->clean();

    mProjectManagementService->unloadProject();
  }
  else {
    PBDev::basicAssert(false);
  }
}

void Photobook::makeCollages()
{
  auto maybeProject = mProjectManagementService->maybeLoadedProjectInfo();
  PBDev::basicAssert(maybeProject != nullptr);

  mCollageTemplateManager->configurePlatformInfo(mPlatformInfo);

  mLutService->configureProject(maybeProject);

  mCollageTemplateManager->configureProject(maybeProject);
  mExportService->configureProject(maybeProject);
  mEffectsService->configureProject(maybeProject);
  mImageFactory->configureProject(maybeProject);
  mImageToPaperService->configureProject(maybeProject);
  mImportLogic->configureProject(maybeProject);

  auto collageThumbnailsMakerListener =
      dynamic_cast<CollageThumbnailsMakerListener *>(this);
  PBDev::basicAssert(collageThumbnailsMakerListener != nullptr);
  mCollageTemplateManager->configureThumbnailsListener(
      collageThumbnailsMakerListener);

  mCollageTemplateManager->generateTemplatesImages();
}

void Photobook::removeImportFolder(Path path)
{
  auto maybeProject = mProjectManagementService->maybeLoadedProjectInfo();
  PBDev::basicAssert(maybeProject != nullptr);
  // TODO: Is pending should not be here, it should be in the import serv
  /*if (maybeProject->value.imageMonitor()->isPending(path)) {
    mParent->onError(PBDev::Error() << PB::ErrorCode::WaitForLoadingCompletion);
  }
  else {*/
  Noir::inst().getLogger()->info("Remove imported folder {}", path.string());
  maybeProject->value.imageMonitor()->removeRow(path);
  //}
}

void Photobook::onProjectRecalled() {}

void Photobook::onProjectMetadataRecalled(std::string focusedProjectName)
{
  mParent->onMetadataUpdated(focusedProjectName);
}

std::shared_ptr<CollageService> Photobook::collageService()
{
  return mCollageTemplateManager;
}

std::shared_ptr<ImportFoldersService> Photobook::importFoldersService()
{
  return mImportLogic;
}

void Photobook::onMappingFinished(Path root, std::vector<Path> newFiles)
{
  auto maybeProject = mProjectManagementService->maybeLoadedProjectInfo();
  PBDev::basicAssert(maybeProject != nullptr);

  std::vector<GenericImagePtr> imagesSet;

  for (auto i = 0; i < newFiles.size(); ++i) {
    auto image = mImageFactory->createImage(newFiles.at(i));
    imagesSet.push_back(image);
  }

  // maybeProject->value.imageMonitor()->addRow(root, imagesSet);

  mParent->onMappingFinished(root, (unsigned)newFiles.size());
}

void Photobook::onSearchingFinished(
    Path           root, std::unordered_map<PBDev::ImageId, GenericImagePtr,
                                            boost::hash<PBDev::ImageId>>
                   placeholders)
{
  auto maybeProject = mProjectManagementService->maybeLoadedProjectInfo();
  PBDev::basicAssert(maybeProject != nullptr);

  maybeProject->value.imageMonitor()->addRow(root, placeholders);

  auto thumbnailsPath = platformInfo()->projectSupportFolder(maybeProject->id) /
  "thumbnail-images";
  post([this, root, placeholders, thumbnailsPath]() {
    mParent->onSearchingFinished(root, placeholders, thumbnailsPath);
  });
}

void Photobook::onImageProcessed(PBDev::ImageId imageId, Path root,
                                 GenericImagePtr image)
{
  auto maybeProject = mProjectManagementService->maybeLoadedProjectInfo();
  PBDev::basicAssert(maybeProject != nullptr);
  maybeProject->value.imageMonitor()->updateImage(imageId, image);

  auto [row, index] = maybeProject->value.imageMonitor()->position(imageId);

  post([this, root, row, index]() {
    mParent->onImageUpdated(root, row, index);
  });
}

void Photobook::onImageProcessingJobEnded(Path root) {}

void Photobook::post(std::function<void()> f) { mParent->post(f); }

std::vector<Path> Photobook::pendingMappingPathList() const
{
  // return mImportLogic.pendingMappingFolders();
  return std::vector<Path>();
}

std::string Photobook::projectName() const
{
  auto maybeLoadedProjectInfo =
      mProjectManagementService->maybeLoadedProjectInfo();
  PBDev::basicAssert(maybeLoadedProjectInfo != nullptr);
  return maybeLoadedProjectInfo->value.name;
}

std::shared_ptr<PlatformInfo> Photobook::platformInfo() const
{
  return mPlatformInfo;
}

std::shared_ptr<ImageToPaperService> Photobook::imageToPaperService() const
{
  return mImageToPaperService;
}

std::shared_ptr<ImageFactory> Photobook::imageFactory() const
{
  return mImageFactory;
}

std::shared_ptr<ProjectManagementService>
Photobook::projectManagementService() const
{
  return mProjectManagementService;
}

std::shared_ptr<LutService> Photobook::lutService() const
{
  return mLutService;
}

std::shared_ptr<ExportService> Photobook::exportService() const
{
  return mExportService;
}

void Photobook::onExportComplete(Path) {}

void Photobook::onExportAborted(Path) {}

void Photobook::onExportUpdate(Path) {}

void Photobook::progressUpdate(PB::ProgressStatus status)
{
  mParent->onProgressUpdate(status);
}

void Photobook::onThumbnailsCreated()
{
  post([this]() { mParent->onCollageThumbnailsCreated(); });
}

void Photobook::onCollageThumbnailsMakerError() {}

void Photobook::onCollageCreated(GenericImagePtr aggregatedImage)
{
  auto maybeProject = projectManagementService()->maybeLoadedProjectInfo();
  PBDev::basicAssert(maybeProject != nullptr);
  auto thumbnailsPath = platformInfo()->projectSupportFolder(maybeProject->id) /
                        "thumbnail-images";
  post([this, aggregatedImage{aggregatedImage},
        thumbnailsPath{thumbnailsPath}]() {
    mParent->onCollageCreated(aggregatedImage, thumbnailsPath);
  });
}

void Photobook::onCollageMakerError() {}

void Photobook::onImportError(PBDev::Error error)
{
  post([this, error]() { mParent->onError(error); });
}

void Photobook::onImageMapped(PBDev::ImageToPaperId id, GenericImagePtr image)
{
  auto maybeProject = projectManagementService()->maybeLoadedProjectInfo();
  PBDev::basicAssert(maybeProject != nullptr);
  auto thumbnailsPath = platformInfo()->projectSupportFolder(maybeProject->id) /
                        "thumbnail-images";
  post([this, id{id}, image{image}, thumbnailsPath{thumbnailsPath}]() {
    mParent->onImageMapped(id, image, thumbnailsPath);
  });
}

void Photobook::onImageCopied(PBDev::ImageToPaperId imageId,
                              GenericImagePtr       image)
{
  auto maybeProject = projectManagementService()->maybeLoadedProjectInfo();
  PBDev::basicAssert(maybeProject != nullptr);
  auto thumbnailsPath = platformInfo()->projectSupportFolder(maybeProject->id) /
                        "thumbnail-images";
  post([this, imageId, image, thumbnailsPath]() {
    mParent->onImageCopied(imageId, image, thumbnailsPath);
  });
}

[[deprecated]] void Photobook::onLutAdded(LutIconInfo iconInfo)
{
  post([this, iconInfo{iconInfo}]() {
    mNoirListener->onNoirLutAdded(iconInfo);
  });
}

void Photobook::onLutApplied(PBDev::LutApplicationId lutId,
                             GenericImagePtr         image)
{
  auto maybeProject = projectManagementService()->maybeLoadedProjectInfo();
  PBDev::basicAssert(maybeProject != nullptr);
  auto thumbnailsPath = platformInfo()->projectSupportFolder(maybeProject->id) /
                        "thumbnail-images";
  post([this, lutId, image, thumbnailsPath]() {
    mParent->onLutApplied(lutId, image, thumbnailsPath);
  });
}

void Photobook::onLutAppliedInMemory(PBDev::LutApplicationId  lutId,
                                     std::shared_ptr<cv::Mat> image)
{
  post([this, lutId, image]() { mParent->onLutAppliedInMemory(lutId, image); });
}

void Photobook::onLutAppliedOnDiskInplace(PBDev::LutApplicationId applicationId)
{
  post([this, applicationId]() {
    mParent->onLutAppliedOnDiskInplace(applicationId);
  });
}

void Photobook::onLutAppliedOnDisk(PBDev::LutApplicationId lutId,
                                   GenericImagePtr         image)
{
  auto maybeProject = projectManagementService()->maybeLoadedProjectInfo();
  PBDev::basicAssert(maybeProject != nullptr);
  auto thumbnailsPath = mPlatformInfo->projectSupportFolder(maybeProject->id) /
                        "thumbnail-images";
  post([this, lutId, image, thumbnailsPath]() {
    mParent->onLutAppliedOnDisk(lutId, image, thumbnailsPath);
  });
}

void Photobook::onEffectsApplied(PBDev::EffectId effectId,
                                 GenericImagePtr image)
{
  auto maybeProject = projectManagementService()->maybeLoadedProjectInfo();
  PBDev::basicAssert(maybeProject != nullptr);
  auto thumbnailsPath = mPlatformInfo->projectSupportFolder(maybeProject->id) /
                        "thumbnail-images";
  post([this, effectId, image, thumbnailsPath]() {
    mParent->onEffectsApplied(effectId, image, thumbnailsPath);
  });
}

void Photobook::onEffectsAppliedInplace(PBDev::EffectId effectId)
{
  post([this, effectId]() { mParent->onEffectsAppliedInplace(effectId); });
}

void Photobook::onEffectsApplicationError(PBDev::EffectId effectId,
                                          PB::ErrorCode)
{
  post([this, effectId]() {
    mParent->onError(PBDev::Error() << PB::ErrorCode::CannotApplyEffect);
  });
}

void onFoundFile(PBDev::DirectoryInspectionJobId id, Path file) { UNUSED(id); }

void onInspectionFinished(PBDev::DirectoryInspectionJobId id,
                          std::vector<Path>               searchResults)
{
  UNUSED(id);
  UNUSED(searchResults);
}

std::string Photobook::help(std::string name) const
{
  auto othersPath = mPlatformInfo->othersPath();
  const std::unordered_map<std::string, std::string> files = {
      {"help", "help.txt"},
      {"license", "License.txt"},
      {"ppolicy", "PrivacyPolicy.txt"}};

  PBDev::basicAssert(files.find(name) != files.end());

  std::ifstream file(othersPath / files.at(name));
  if (!file) {
    PBDev::basicAssert(false);
  }
  std::string content;
  std::string line;
  while (std::getline(file, line)) {
    content += line + '\n';
  }

  file.close();

  return content;
}

std::shared_ptr<EffectsService> Photobook::effectsService() const
{
  return mEffectsService;
}

} // namespace PB
