#include <pb/PhotoBook.h>

#include <pb/ImportImageTask.h>
#include <pb/export/PdfLibharu.h>
#include <pb/export/PdfPoDoFo.h>
#include <pb/image/Image.h>
#include <pb/image/ImageFactory.h>

namespace PB {

// TODO: Break this into multiple methods
Photobook::Photobook(Path localStatePath, Path installationPath,
                     std::pair<unsigned, unsigned> screenSize)
    : mTaskCruncher(std::make_shared<TaskCruncher>()),
      mPlatformInfo(std::make_shared<PlatformInfo>(installationPath,
                                                   localStatePath, screenSize)),
      mPersistenceService(std::make_shared<PersistenceService>(mPlatformInfo)),
      mImportLogic(mPlatformInfo),
      mCollageTemplateManager(
          std::make_shared<CollageManager>(localStatePath, installationPath))
{
  ImageFactory::inst().configurePlatformInfo(mPlatformInfo);
  ImageFactory::inst().configurePersistenceService(mPersistenceService);

  auto importFoldersLogicListener =
      dynamic_cast<ImportFoldersLogicListener *>(this);
  PBDev::basicAssert(importFoldersLogicListener != nullptr);
  mImportLogic.configure(importFoldersLogicListener);

  auto threadScheduler = dynamic_cast<PBDev::ThreadScheduler *>(this);
  PBDev::basicAssert(threadScheduler != nullptr);
  mImportLogic.configure(threadScheduler);

  auto persistenceServiceListener =
      dynamic_cast<PersistenceServiceListener *>(this);
  PBDev::basicAssert(persistenceServiceListener != nullptr);
  mPersistenceService->configure(persistenceServiceListener);

  auto progressManagerListener =
      dynamic_cast<PB::ProgressManagerListener *>(this);
  PBDev::basicAssert(progressManagerListener != nullptr);
  mProgressManager.configure(progressManagerListener);

  auto collageThumbnailsMakerListener =
      dynamic_cast<PB::CollageThumbnailsMakerListener *>(this);
  PBDev::basicAssert(collageThumbnailsMakerListener != nullptr);
  mCollageTemplateManager->configureListener(collageThumbnailsMakerListener);

  auto collageMakerListener = dynamic_cast<PB::CollageMakerListener *>(this);
  PBDev::basicAssert(collageMakerListener != nullptr);
  mCollageTemplateManager->configureCollageMakerListener(collageMakerListener);

  mPersistenceService->configure(localStatePath);

  mTaskCruncher->registerPTC("image-search-job", 1);
  mTaskCruncher->registerPTC("export-logic", 1);
  mTaskCruncher->registerPTC("collage-thumbnails", 1);
  mTaskCruncher->registerPTC("upl-to-spl-map", 4);

  auto exportListener = dynamic_cast<PB::ExportListener *>(this);
  PBDev::basicAssert(exportListener != nullptr);
  mExportLogic.setExportListener(exportListener);

  mImportLogic.setTaskCruncher(mTaskCruncher);
  mExportLogic.setTaskCruncher(mTaskCruncher);
  mCollageTemplateManager->setTaskCruncher(mTaskCruncher);

  mImageToPaperService->configurePersistenceService(mPersistenceService);
  mImageToPaperService->configurePlatformInfo(mPlatformInfo);
  mImageToPaperService->configureTaskCruncher(mTaskCruncher);
}

void Photobook::configure(PhotobookListener *listener) { mParent = listener; }

void Photobook::configure(StagedImagesListener *listener)
{
  mImageViews.stagedImages().setListener(listener);
}

void Photobook::configure(ImageMonitorListener *listener)
{
  mImageViews.imageMonitor().setListener(listener);
}

void Photobook::configure(std::shared_ptr<PB::Project> project)
{
  mImportLogic.configure(mPersistenceService->currentProject());
  ImageFactory::inst().configureProject(mPersistenceService->currentProject());
  mExportLogic.configure(mPersistenceService->currentProject(), mPlatformInfo);
  mImageToPaperService->configureProject(mPersistenceService->currentProject());
}

void Photobook::configureCurrentProject()
{
  configure(project()->currentProject());
}

void Photobook::unloadProject()
{
  mImportLogic.stopAll();
  if (mImportLogic.runningImageProcessingJobs().empty()) {
    mPersistenceService->clear();
    mImageViews.imageMonitor().clear();
    mImageViews.stagedImages().clear();
  }
  else {
    mMarkProjectForDeletion = true;
  }
}

void Photobook::recallMetadata() { mPersistenceService->recallMetadata(); }

void Photobook::recallProject(std::string name)
{
  mProjectName = name;
  mPersistenceService->recallProject(name);
}

void Photobook::addImportFolder(Path path)
{
  if (mImageViews.imageMonitor().containsRow(path, true)) {
    mParent->onError(PBDev::Error() << PB::ErrorCode::FolderAlreadyImported);
    return;
  }

  auto maybeError = mImportLogic.addImportFolder(path);

  if (maybeError) {
    mParent->onError(maybeError.value());
    return;
  }
}

void Photobook::removeImportFolder(Path path)
{
  if (mImportLogic.marked(path)) {
    return;
  }

  mImportLogic.stop(path);

  if (mImageViews.imageMonitor().isPending(path)) {
    mImportLogic.markForDeletion(path);
  }
  else {
    mImageViews.imageMonitor().removeRow(path);
  }
}

void Photobook::onError(PBDev::Error error) { mParent->onError(error); }

void Photobook::exportPDFAlbum(std::string name, Path path)
{
  auto pdfName = path / (name + ".pdf");

  std::shared_ptr<PdfExportTask> task = std::make_shared<PdfExportTask>(
      pdfName, mPlatformInfo->localStatePath,
      mPersistenceService->currentProject()->paperSettings,
      mImageViews.stagedImages().stagedPhotos());

  task->setListener(&mExportLogic);
  mProgressManager.subscribe(task->name(), JobType::ExportPdf,
                             task->stepsCount());

  mExportLogic.start(task->name(), std::static_pointer_cast<MapReducer>(task));
}

void Photobook::exportPDFLibharu(std::string name, Path path)
{
  auto pdfName = path / (name + "-libharu" + ".pdf");

  std::shared_ptr<PdfLibharuExportTask> task =
      std::make_shared<PdfLibharuExportTask>(
          pdfName, mPlatformInfo->localStatePath,
          mPersistenceService->currentProject()->paperSettings,
          mImageViews.stagedImages().stagedPhotos());

  task->setListener(&mExportLogic);
  mProgressManager.subscribe(task->name(), JobType::ExportLibharu,
                             task->stepsCount());
  mExportLogic.start(task->name(), std::static_pointer_cast<MapReducer>(task));
}

void Photobook::exportJPGAlbum(std::string name, Path path)
{
  auto newFolder = path / name;
  if (std::filesystem::exists(newFolder)) {
    mParent->onError(PBDev::Error()
                     << ErrorCode::CannotExport << newFolder.string());
  }
  else {
    auto success = std::filesystem::create_directories(newFolder);
    PBDev::basicAssert(success);

    std::shared_ptr<JpgExport> task = std::make_shared<JpgExport>(
        newFolder, mPersistenceService->currentProject()->paperSettings,
        mImageViews.stagedImages().stagedPhotos());

    task->setListener(&mExportLogic);
    mProgressManager.subscribe(task->name(), JobType::ExportJpg,
                               task->stepsCount());
    mExportLogic.start(task->name(),
                       std::static_pointer_cast<MapReducer>(task));
  }
}

std::shared_ptr<PersistenceService> Photobook::project()
{
  return mPersistenceService;
}

ImageViews &Photobook::imageViews() { return mImageViews; }

void Photobook::onProjectRead(
    std::vector<std::vector<std::shared_ptr<VirtualImage>>> &unstagedImages,
    std::vector<std::shared_ptr<VirtualImage>>              &stagedImages,
    std::vector<Path>                                       &roots)
{
  configure(mPersistenceService->currentProject());
  mImageViews.imageMonitor().replaceImageMonitorData(unstagedImages, roots);
  mImageViews.stagedImages().configure(stagedImages);

  mImageToPaperService->configureProject(mPersistenceService->currentProject());

  auto collageThumbnailsMakerListener =
      dynamic_cast<PB::CollageThumbnailsMakerListener *>(this);
  PBDev::basicAssert(collageThumbnailsMakerListener != nullptr);
  mCollageTemplateManager->configureProject(project()->currentProject());
  mCollageTemplateManager->generateTemplatesImages();
  mCollageTemplateManager->configureProjectId(
      boost::uuids::to_string(project()->currentProjectUUID()));

  mParent->onProjectRead();

  auto unprocessedImages = mImageViews.imageMonitor().unprocessedImages();

  for (auto &unprocessedImage : unprocessedImages) {
    mProgressManager.subscribe(unprocessedImage.root.string(),
                               JobType::ThumbnailsProcess,
                               (int)unprocessedImage.images.size());
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

void Photobook::newProject(std::string name, PaperSettings paperSettings)
{
  mProjectName = name;
  // TODO: mPersistenceService should announce mImportLogic when the project was
  // updated
  auto newProject = std::make_shared<Project>();
  newProject->paperSettings = paperSettings;

  mPersistenceService->newProject(name, newProject);
  mImportLogic.configure(mPersistenceService->currentProject());
  ImageFactory::inst().configureProject(mPersistenceService->currentProject());
  mImageToPaperService->configureProject(mPersistenceService->currentProject());
}

void Photobook::onMappingStarted(Path path)
{
  mParent->onMappingStarted(path);
  mProgressManager.subscribe(path.string(), JobType::Map);
}

void Photobook::onMappingAborted(Path path)
{
  mParent->onMappingAborted(path);
  mProgressManager.abort(path.string());
}

std::shared_ptr<CollageManager> Photobook::collageManager()
{
  return mCollageTemplateManager;
}

void Photobook::onMappingFinished(Path root, std::vector<Path> newFiles)
{
  std::vector<std::shared_ptr<VirtualImage>> imagesSet;

  std::vector<ProcessingData> keyAndPaths;

  for (auto i = 0; i < newFiles.size(); ++i) {
    auto virtualImage = PB::ImageFactory::inst().createImage(newFiles.at(i));
    imagesSet.push_back(virtualImage);
    keyAndPaths.push_back({virtualImage->frontend().full,
                           virtualImage->frontend().full, (unsigned)i});
  }

  mImageViews.imageMonitor().addRow(root, imagesSet);

  mParent->onMappingFinished(root);
  mProgressManager.finish(root.string());

  mProgressManager.subscribe(root.string(), JobType::ThumbnailsProcess,
                             (int)keyAndPaths.size());

  RowProcessingData rowProcessingData = {root, keyAndPaths};
  auto imageHash = mPersistenceService->hash(rowProcessingData.root);
  mImportLogic.processImages(
      boost::uuids::to_string(project()->currentProjectUUID()),
      rowProcessingData, imageHash.stem().string());
}

void Photobook::onImportStop(Path) {}

void Photobook::onImageProcessed(Path key, Path root,
                                 ImageResources imageResources)
{
  mImageViews.imageMonitor().image(key)->setSizePath(
      imageResources.full, imageResources.medium, imageResources.small);
  mImageViews.imageMonitor().image(key)->setSize(imageResources.width,
                                                 imageResources.height);
  mImageViews.imageMonitor().image(key)->finishProcessing();

  auto [progress, progressCap] = mImportLogic.imageProcessingProgress(root);
  auto [globalProgress, globalProgressCap] =
      mImportLogic.imageProcessingProgress();

  mProgressManager.update(root.string());

  auto [row, index] = mImageViews.imageMonitor().position(key);

  mParent->onImageUpdated(root, row, index);

  if (progress == progressCap) {
    auto rowIndex = mImageViews.imageMonitor().rowIndex(root);
    mImageViews.imageMonitor().completeRow(rowIndex);

    mImportLogic.clearJob(root);

    if (mImportLogic.marked(root)) {
      mImageViews.imageMonitor().removeRow(root);
      mImportLogic.removeMarkForDeletion(root);
    }

    if (mMarkProjectForDeletion) {
      mMarkProjectForDeletion = false;
      mImageViews.imageMonitor().clear();
    }
  }
}

void Photobook::onImageProcessingJobEnded(Path root) {}

void Photobook::post(std::function<void()> f) { mParent->post(f); }

std::vector<Path> Photobook::pendingMappingPathList() const
{
  return mImportLogic.pendingMappingFolders();
}

std::string Photobook::projectName() const { return mProjectName; }

std::shared_ptr<PlatformInfo> Photobook::platformInfo() const
{
  return mPlatformInfo;
}

void Photobook::onProjectRenamed() {}

void Photobook::onExportComplete(std::string name) {}

void Photobook::onExportAborted(std::string name)
{
  mProgressManager.abort(name);
}

void Photobook::onExportUpdate(std::string name)
{
  mProgressManager.update(name);
}

void Photobook::progressUpdate(PB::ProgressInfo definedProgress,
                               PB::ProgressInfo undefinedProgress)
{
  post([this, definedProgress{definedProgress},
        undefinedProgress{undefinedProgress}]() {
    mParent->onProgressUpdate(definedProgress, undefinedProgress);
  });
}

void Photobook::onThumbnailsCreated()
{
  post([this]() { mParent->onCollageThumbnailsCreated(); });
}

void Photobook::onCollageThumbnailsMakerError() {}

void Photobook::onCollageCreated(unsigned index, Path imagePath)
{
  auto imageHash = mPersistenceService->hash(imagePath);

  auto [smallPath, mediumPath] = ThumbnailsProcessor::assembleOutputPaths(
      mPlatformInfo->localStatePath, 0, imageHash.stem().string(),
      boost::uuids::to_string(project()->currentProjectUUID()));

  auto newImage = ImageFactory::inst().createRegularImage(imagePath);

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
}

void Photobook::onCollageMakerError() {}

} // namespace PB