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

  mOGLEngine->configurePlatformInfo(mPlatformInfo);

  mProgressService->configureScheduler(threadScheduler);

  mTaskCruncher->configureProgressService(mProgressService);

  auto exportListener = dynamic_cast<PB::ExportListener *>(this);
  PBDev::basicAssert(exportListener != nullptr);
  mExportLogic.setExportListener(exportListener);

  mDatabaseService->configurePlatformInfo(mPlatformInfo);

  mDurableHashService->configureDatabaseService(mDatabaseService);

  mProjectManagementService->configurePlatformInfo(mPlatformInfo);
  mProjectManagementService->configureProjectSerializerService(
      mProjectSerializerService);

  mProjectSerializerService->configurePlatformInfo(mPlatformInfo);

  mImportLogic->configureTaskCruncher(mTaskCruncher);
  mImportLogic->configurePlatformInfo(mPlatformInfo);
  mImportLogic->configureProjectManagementService(mProjectManagementService);
  mImportLogic->configureListener(importFoldersServiceListener);
  mImportLogic->configureScheduler(threadScheduler);

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
  mImageFactory->configureProjectManagementService(mProjectManagementService);
  mImageFactory->configureDurableHashService(mDurableHashService);

  mProjectManagementService->configureDatabaseService(mDatabaseService);

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

  mCollageTemplateManager->configureProject(maybeProject);
  auto collageThumbnailsMakerListener =
      dynamic_cast<CollageThumbnailsMakerListener *>(this);
  PBDev::basicAssert(collageThumbnailsMakerListener != nullptr);
  mCollageTemplateManager->configureThumbnailsListener(
      collageThumbnailsMakerListener);

  mCollageTemplateManager->generateTemplatesImages();
}

void Photobook::addImportFolder(Path path)
{
  auto maybeProject = mProjectManagementService->maybeLoadedProjectInfo();
  PBDev::basicAssert(maybeProject != nullptr);
  if (maybeProject->second.imageMonitor()->containsRow(path, true)) {
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
  auto maybeProject = mProjectManagementService->maybeLoadedProjectInfo();
  PBDev::basicAssert(maybeProject != nullptr);

  if (maybeProject->second.imageMonitor()->isPending(path)) {
    mParent->onError(PBDev::Error() << PB::ErrorCode::WaitForLoadingCompletion);
  }
  else {
    Noir::inst().getLogger()->info("Remove imported folder {}", path.string());
    maybeProject->second.imageMonitor()->removeRow(path);
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
      maybeProject->second.stagedImages()->stagedPhotos());

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
          maybeProject->second.stagedImages()->stagedPhotos());

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
        maybeProject->second.stagedImages()->stagedPhotos());

    task->setListener(&mExportLogic);
    mExportLogic.start(task->name(),
                       std::static_pointer_cast<MapReducer>(task));
  }
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

void Photobook::onMappingFinished(Path root, std::vector<Path> newFiles)
{
  auto maybeProject = mProjectManagementService->maybeLoadedProjectInfo();
  PBDev::basicAssert(maybeProject != nullptr);

  std::vector<GenericImagePtr> imagesSet;

  for (auto i = 0; i < newFiles.size(); ++i) {
    auto image = mImageFactory->createImage(newFiles.at(i));
    imagesSet.push_back(image);
  }

  maybeProject->second.imageMonitor()->addRow(root, imagesSet);

  mParent->onMappingFinished(root);
}

void Photobook::onImageProcessed(Path key, Path root,
                                 GenericImagePtr imageResources)
{
  auto maybeProject = mProjectManagementService->maybeLoadedProjectInfo();
  PBDev::basicAssert(maybeProject != nullptr);
  maybeProject->second.imageMonitor()->replaceImage(root, imageResources, -1);
  auto [row, index] =
      maybeProject->second.imageMonitor()->position(imageResources->full());

  if (mImportLogic->isFinished(root)) {
    maybeProject->second.imageMonitor()->completeRowByPath(root);
  }

  mParent->onImageUpdated(root, row, index);
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

std::shared_ptr<LutService> Photobook::lutService() const
{
  return mLutService;
}

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

// TODO: This and ImageToPaperTask need to be refactored, move to
// CollageMakerJob
void Photobook::onCollageCreated(unsigned index, Path imagePath)
{
  auto maybeLoadedProjectInfo =
      mProjectManagementService->maybeLoadedProjectInfo();
  PBDev::basicAssert(maybeLoadedProjectInfo != nullptr);

  auto coreHash = mDurableHashService->getHash(
      PBDev::ProjectId(maybeLoadedProjectInfo->first), imagePath);

  auto imageHash = mPlatformInfo->thumbnailByHash(maybeLoadedProjectInfo->first,
                                                  coreHash, ".jpg");

  auto newHash = boost::uuids::to_string(boost::uuids::random_generator()());

  auto maybeNewHash = ThumbnailsTask::createThumbnailsByPath(
      imagePath, mPlatformInfo, maybeLoadedProjectInfo, newHash);

  PBDev::basicAssert(maybeNewHash == newHash);
  auto newImage = mImageFactory->createRegularImage(imagePath);

  std::function<void(unsigned, unsigned)> onFinished =
      [this, index{index}, newImage{newImage}](unsigned width,
                                               unsigned height) {
        post([this, index{index}, newImage{newImage}]() {
          mParent->onCollageCreated(index, newImage);
        });
      };

  ImportImageTask importImageTask(
      newImage->full(), newImage->medium(), newImage->smaLL(), onFinished,
      mPlatformInfo->screenSize.first, mPlatformInfo->screenSize.second,
      std::stop_source().get_token());

  importImageTask();
}

void Photobook::onCollageMakerError() {}

void Photobook::onImportError(PBDev::Error error) { mParent->onError(error); }

void Photobook::onImageMapped(PBDev::ImageToPaperId id, GenericImagePtr image)
{
  post([this, id{id}, image{image}]() { mParent->onImageMapped(id, image); });
}

[[deprecated]] void Photobook::onLutAdded(LutIconInfo iconInfo)
{
  post([this, iconInfo{iconInfo}]() {
    mNoirListener->onNoirLutAdded(iconInfo);
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
  const std::unordered_map<std::string, std::string> files = {
      {"help", "help.txt"}, {"license", "license.txt"}};

  PBDev::basicAssert(files.find(name) != files.end());

  std::ifstream file(files.at(name));
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

} // namespace PB
