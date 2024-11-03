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
      mProgressService(std::make_shared<ProgressService>()),
      mImageToPaperService(std::make_shared<ImageToPaperService>()),
      mCollageTemplateManager(std::make_shared<CollageService>()),
      mLutService(std::make_shared<LutService>()),
      mOGLEngine(std::make_shared<OGLEngine>())
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

  auto imageToPaperServiceListener =
      dynamic_cast<PB::ImageToPaperServiceListener *>(this);
  PBDev::basicAssert(imageToPaperServiceListener != nullptr);
  mImageToPaperService->setImageToPaperServiceListener(
      imageToPaperServiceListener);

  auto collageThumbnailsMakerListener =
      dynamic_cast<CollageThumbnailsMakerListener *>(this);
  PBDev::basicAssert(collageThumbnailsMakerListener != nullptr);
  mCollageTemplateManager->configureThumbnailsListener(
      collageThumbnailsMakerListener);

  auto lutServiceListener = dynamic_cast<LutServiceListener *>(this);
  PBDev::basicAssert(lutServiceListener != nullptr);
  mLutService->configureLutServiceListener(lutServiceListener);

  auto collageMakerListener = dynamic_cast<CollageMakerListener *>(this);
  PBDev::basicAssert(collageMakerListener != nullptr);
  mCollageTemplateManager->configureCollageMakerListener(collageMakerListener);

  auto projectManagementServiceListener =
      dynamic_cast<ProjectManagementServiceListener *>(this);
  PBDev::basicAssert(projectManagementServiceListener != nullptr);
  mProjectManagementService->configureProjectManagementServiceListener(
      projectManagementServiceListener);

  mCollageTemplateManager->configurePlatformInfo(mPlatformInfo);
#ifdef __APPLE__
  mOGLEngine->configureThreadScheduler(threadScheduler);
#endif
  mOGLEngine->configurePlatformInfo(mPlatformInfo);

  mProgressService->configureScheduler(threadScheduler);

  mTaskCruncher->configureProgressService(mProgressService);

  auto exportListener = dynamic_cast<PB::ExportListener *>(this);
  PBDev::basicAssert(exportListener != nullptr);
  mExportLogic.setExportListener(exportListener);

  mDatabaseService->configurePlatformInfo(mPlatformInfo);

  mDurableHashService->configureDatabaseService(mDatabaseService);

  mProjectManagementService->configurePlatformInfo(mPlatformInfo);
  mProjectManagementService->configureProjectSerializerService(mProjectSerializerService);

  mImportLogic->configureTaskCruncher(mTaskCruncher);
  mImportLogic->configurePlatformInfo(mPlatformInfo);
  mImportLogic->configureProjectManagementService(mProjectManagementService);
  mImportLogic->configureListener(importFoldersServiceListener);

  mExportLogic.setTaskCruncher(mTaskCruncher);
  mCollageTemplateManager->configureTaskCruncher(mTaskCruncher);

  mLutService->configurePlatformInfo(mPlatformInfo);
  mLutService->configureTaskCruncher(mTaskCruncher);
  mLutService->configureThreadScheduler(threadScheduler);
  mLutService->configureOGLEngine(mOGLEngine);
  mLutService->configureDurableHashService(mDurableHashService);

  mImageToPaperService->configurePlatformInfo(mPlatformInfo);
  mImageToPaperService->configureTaskCruncher(mTaskCruncher);
  mImageToPaperService->configureProjectManagementService(
      mProjectManagementService);

  mImageFactory->configurePlatformInfo(mPlatformInfo);

  mProjectManagementService->configureDatabaseService(mDatabaseService);

  for (auto const &[poolName, poolSize] : OneConfig::TASK_CRUNCHER_POOLS_INFO) {
    mTaskCruncher->registerPTC(poolName, poolSize);
  }
}

Photobook ::~Photobook() { mDatabaseService->disconnect(); }

void Photobook::initLogger()
{
  try {
#ifdef WIN32
    OneConfig::LOGGER = std::make_shared<spdlog::logger>(
        "msvc_logger", std::make_shared<spdlog::sinks::msvc_sink_mt>());
    OneConfig::LOGGER->set_level(spdlog::level::debug);
    OneConfig::LOGGER->info("Log init succeeded");
#endif
  }
  catch (const spdlog::spdlog_ex &ex) {
    std::cout << "Log init failed: " << ex.what() << std::endl;
  }
}

void Photobook::configure(PhotobookListener *listener) { mParent = listener; }

void Photobook::startPhotobook()
{
  mDatabaseService->connect();
  mDatabaseService->maybeCreateTables();
  mOGLEngine->start(OneConfig::BG_CONTROL.at("ogl-engine").get_token());
  mLutService->startLutService();
}

void Photobook::unloadProject()
{
  /*
  if (mImportLogic.runningImageProcessingJobs().empty()) {
    mProjectManagementSystem->unloadProject();
  }
  else {
    mMarkProjectForDeletion = true;
  }
  */
}

void Photobook::recallMetadata()
{
  mProjectManagementService->recallMetadata();
}

void Photobook::recallProject(std::string name)
{
  auto metadata = mProjectManagementService->metadata();
  auto projectId = metadata.right.at(name);

  mProjectManagementService->loadProject(projectId);
}

void Photobook::addImportFolder(Path path)
{
  auto maybeProject = mProjectManagementService->maybeLoadedProjectInfo();
  PBDev::basicAssert(maybeProject != nullptr);
  if (maybeProject->second.imageMonitor().containsRow(path, true)) {
    mParent->onError(PBDev::Error() << PB::ErrorCode::FolderAlreadyImported);
    return;
  }

  auto maybeError = mImportLogic->addImportFolder(path);

  if (maybeError) {
    mParent->onError(maybeError.value());
    return;
  }
}

void Photobook::removeImportFolder(Path path)
{
  // if (mImportLogic.marked(path)) {
  //   return;
  // }

  auto maybeProject = mProjectManagementService->maybeLoadedProjectInfo();
  PBDev::basicAssert(maybeProject != nullptr);

  if (maybeProject->second.imageMonitor().isPending(path)) {
    // mImportLogic.markForDeletion(path);
  }
  else {
    maybeProject->second.imageMonitor().removeRow(path);
  }
}

void Photobook::exportPDFAlbum(std::string name, Path path)
{
  auto maybeProject = mProjectManagementService->maybeLoadedProjectInfo();
  PBDev::basicAssert(maybeProject != nullptr);

  auto pdfName = path / (name + ".pdf");

  std::shared_ptr<PdfExportTask> task = std::make_shared<PdfExportTask>(
      pdfName, mPlatformInfo->localStatePath,
      maybeProject->second.paperSettings,
      maybeProject->second.stagedImages().stagedPhotos());

  task->setListener(&mExportLogic);

  mExportLogic.start(task->name(), std::static_pointer_cast<MapReducer>(task));
}

void Photobook::exportPDFLibharu(std::string name, Path path)
{
  auto pdfName = path / (name + "-libharu" + ".pdf");

  auto maybeProject = mProjectManagementService->maybeLoadedProjectInfo();
  PBDev::basicAssert(maybeProject != nullptr);

  std::shared_ptr<PdfLibharuExportTask> task =
      std::make_shared<PdfLibharuExportTask>(
          pdfName, mPlatformInfo->localStatePath,
          maybeProject->second.paperSettings,
          maybeProject->second.stagedImages().stagedPhotos());

  task->setListener(&mExportLogic);
  mExportLogic.start(task->name(), std::static_pointer_cast<MapReducer>(task));
}

void Photobook::exportJPGAlbum(std::string name, Path path)
{
  auto maybeProject = mProjectManagementService->maybeLoadedProjectInfo();
  PBDev::basicAssert(maybeProject != nullptr);
  auto newFolder = path / name;
  if (std::filesystem::exists(newFolder)) {
    mParent->onError(PBDev::Error()
                     << ErrorCode::CannotExport << newFolder.string());
  }
  else {
    auto success = std::filesystem::create_directories(newFolder);
    PBDev::basicAssert(success);
    std::shared_ptr<JpgExport> task = std::make_shared<JpgExport>(
        newFolder, maybeProject->second.paperSettings,
        maybeProject->second.stagedImages().stagedPhotos());

    task->setListener(&mExportLogic);
    mExportLogic.start(task->name(),
                       std::static_pointer_cast<MapReducer>(task));
  }
}

/*
void Photobook::onProjectRead(
    std::vector<std::vector<std::shared_ptr<GenericImagePtr>>> &unstagedImages,
    std::vector<std::shared_ptr<GenericImagePtr>>              &stagedImages,
    std::vector<Path>                                       &roots)
{
  mImageViews.imageMonitor().replaceImageMonitorData(unstagedImages, roots);
  mImageViews.stagedImages().configure(stagedImages);

  mImageToPaperService->configureProject(mPersistenceService->currentProject());

  auto collageThumbnailsMakerListener =
      dynamic_cast<PB::CollageThumbnailsMakerListener *>(this);
  PBDev::basicAssert(collageThumbnailsMakerListener != nullptr);
  auto currentProject = mPersistenceService->currentProject();
  mCollageTemplateManager->configureProject(currentProject);
  mCollageTemplateManager->generateTemplatesImages();
  mCollageTemplateManager->configureProjectId(
      boost::uuids::to_string(project()->currentProjectUUID()));

  mParent->onProjectRead();

  auto unprocessedImages = mImageViews.imageMonitor().unprocessedImages();

  for (auto &unprocessedImage : unprocessedImages) {
    auto imageHash = mPersistenceService->hash(unprocessedImage.root);
    mImportLogic.processImages(
        boost::uuids::to_string(project()->currentProjectUUID()),
        unprocessedImage, imageHash.stem().string());
  }
}

void Photobook::onMetadataUpdated() { mParent->onMetadataUpdated(); }

void Photobook::onPersistenceError(PBDev::Error error)
{
  mParent->onPersistenceError(error);
}
*/

void Photobook::onProjectRecalled() {}

void Photobook::onProjectMetadataRecalled() { mParent->onMetadataUpdated(); }

void Photobook::newProject(std::string name, PaperSettings paperSettings)
{
  auto maybeProject = mProjectManagementService->maybeLoadedProjectInfo();
  PBDev::basicAssert(maybeProject != nullptr);

  mProjectManagementService->newProject(paperSettings);
}

std::shared_ptr<CollageService> Photobook::collageService()
{
  return mCollageTemplateManager;
}

void Photobook::onMappingFinished(Path root, std::vector<Path> newFiles)
{
  mParent->onMappingFinished(root);
  /*
  auto maybeProject = mProjectManagementSystem->maybeLoadedProjectInfo();
  PBDev::basicAssert(maybeProject != nullptr);

  std::vector<std::shared_ptr<GenericImagePtr>> imagesSet;

  std::vector<ProcessingData> keyAndPaths;

  for (auto i = 0; i < newFiles.size(); ++i) {
    auto GenericImagePtr = mImageFactory->createImage(newFiles.at(i));
    imagesSet.push_back(GenericImagePtr);
    keyAndPaths.push_back({GenericImagePtr->frontend().full,
                           GenericImagePtr->frontend().full, (unsigned)i});
  }

  maybeProject->second.imageMonitor().addRow(root, imagesSet);

  mParent->onMappingFinished(root);

  RowProcessingData rowProcessingData = {root, keyAndPaths};

  auto maybeLoadedProjectInfo =
      mProjectManagementSystem->maybeLoadedProjectInfo();
  PBDev::basicAssert(maybeLoadedProjectInfo != nullptr);

  auto coreHash = mDurableHashService->getHash(
      PBDev::ProjectId(maybeLoadedProjectInfo->first), rowProcessingData.root);

  auto imageHash = mPlatformInfo->thumbnailByHash(maybeLoadedProjectInfo->first,
                                                  coreHash, "jpg");
*/
  // mImportLogic.processImages(
  //     boost::uuids::to_string(maybeLoadedProjectInfo->first),
  //     rowProcessingData, imageHash.stem().string());
}

void Photobook::onImageProcessed(Path key, Path root,
                                 GenericImagePtr imageResources)
{
  /*
  auto maybeProject = mProjectManagementService->maybeLoadedProjectInfo();
  PBDev::basicAssert(maybeProject != nullptr);

  maybeProject->second.imageMonitor().image(key)->setSizePath(
      imageResources.full, imageResources.medium, imageResources.small);
  maybeProject->second.imageMonitor().image(key)->setSize(
      imageResources.width, imageResources.height);
  maybeProject->second.imageMonitor().image(key)->finishProcessing();
  */
  /*
  auto [progress, progressCap] = mImportLogic.imageProcessingProgress(root);
  auto [row, index] = maybeProject->second.imageMonitor().position(key);

  mParent->onImageUpdated(root, row, index);

  if (progress == progressCap) {
    auto rowIndex = maybeProject->second.imageMonitor().rowIndex(root);
    maybeProject->second.imageMonitor().completeRow(rowIndex);

    if (mImportLogic.marked(root)) {
      maybeProject->second.imageMonitor().removeRow(root);
      //mImportLogic.removeMarkForDeletion(root);
    }

    if (mMarkProjectForDeletion) {
      mMarkProjectForDeletion = false;
      maybeProject->second.imageMonitor().clear();
    }
  }
  */
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
  return maybeLoadedProjectInfo->second.name;
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

// void Photobook::onProjectRenamed() {}

void Photobook::onExportComplete(std::string name) {}

void Photobook::onExportAborted(std::string name) {}

void Photobook::onExportUpdate(std::string name) {}

void Photobook::progressUpdate(PB::ProgressStatus status)
{
  mParent->onProgressUpdate(status);
}

void Photobook::onThumbnailsCreated()
{
  post([this]() { mParent->onCollageThumbnailsCreated(); });
}

void Photobook::onCollageThumbnailsMakerError() {}

void Photobook::onCollageCreated(unsigned index, Path imagePath)
{
  auto maybeLoadedProjectInfo =
      mProjectManagementService->maybeLoadedProjectInfo();
  PBDev::basicAssert(maybeLoadedProjectInfo != nullptr);

  auto coreHash = mDurableHashService->getHash(
      PBDev::ProjectId(maybeLoadedProjectInfo->first), imagePath);

  auto imageHash = mPlatformInfo->thumbnailByHash(maybeLoadedProjectInfo->first,
                                                  coreHash, ".jpg");
  /*
  auto [smallPath, mediumPath] = ThumbnailsProcessor::assembleOutputPaths(
      mPlatformInfo->localStatePath, 0, imageHash.stem().string(),
      boost::uuids::to_string(maybeLoadedProjectInfo->first));

  auto newImage = mImageFactory->createRegularImage(imagePath);

  std::function<void(unsigned, unsigned)> onFinished =
      [this, index{index}, newImage{newImage}, imagePath{imagePath},
       mediumPath{mediumPath},
       smallPath{smallPath}](unsigned width, unsigned height) {
        newImage->setSizePath(imagePath, mediumPath, smallPath);
        newImage->setSize(width, height);

        post([this, index{index}, newImage{newImage}]() {
          mParent->onCollageCreated(index, newImage);
        });
      };

  ImportImageTask importImageTask(imagePath, mediumPath, smallPath, onFinished,
                                  mPlatformInfo->screenSize.first,
                                  mPlatformInfo->screenSize.second,
                                  std::stop_source().get_token());

  importImageTask();
  */
}

void Photobook::onCollageMakerError() {}

void Photobook::onImageMapped(PBDev::ImageToPaperId id, GenericImagePtr image)
{
  post([this, id{id}, image{image}]() { mParent->onImageMapped(id, image); });
}

void Photobook::onLutAdded(LutIconInfo iconInfo)
{
  post([this, iconInfo{iconInfo}]() { mParent->onLutAdded(iconInfo); });
}

void onFoundFile(PBDev::DirectoryInspectionJobId id, Path file) { UNUSED(id); }

void onInspectionFinished(PBDev::DirectoryInspectionJobId id,
                          std::vector<Path>               searchResults)
{
  UNUSED(id);
  UNUSED(searchResults);
}

} // namespace PB
