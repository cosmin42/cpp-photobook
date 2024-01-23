#include <pb/PhotoBook.h>

#include <pb/image/Image.h>
#include <pb/image/ImageFactory.h>

namespace PB {
Photobook::Photobook(Path localStatePath, Path installationPath)
    : mPlatformInfo(
          std::make_shared<PlatformInfo>(installationPath, localStatePath))
{
  VirtualImage::platformInfo = mPlatformInfo;
  ProjectSnapshot::platformInfo = mPlatformInfo;

  mImportLogic.configure((ImportFoldersLogicListener *)this);
  mImportLogic.configure((ThreadScheduler *)this);

  mProjectPersistence.configure((ProjectPersistenceListener *)this);
  mProjectPersistence.configure(localStatePath);

  mExportLogic.configure(
      (PBDev::SequentialTaskConsumerListener<PdfExportTask> *)this);
}

void Photobook::configure(std::pair<int, int> screenSize)
{
  mImportLogic.configure(screenSize);
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
  mImportLogic.configure(mProjectPersistence.currentProject());
  ImageFactory::inst().configure(mProjectPersistence.currentProject());
  mExportLogic.configure(mProjectPersistence.currentProject(), mPlatformInfo);
}

void Photobook::loadProject()
{
  for (auto &path :
       mProjectPersistence.currentProject()->active().importedPaths) {
    addImportFolder(path);
  }

  loadStagedImages();
}

void Photobook::unloadProject()
{
  mImportLogic.stopAll();
  if (mImportLogic.runningImageProcessingJobs().empty()) {
    mProjectPersistence.clear();
    mImageViews.imageMonitor().clear();
  }
  else {
    mMarkProjectForDeletion = true;
  }
}

void Photobook::recallMetadata() { mProjectPersistence.recallMetadata(); }

void Photobook::recallProject(std::string name)
{
  mProjectPersistence.recallProject(name);
}

void Photobook::addImportFolder(Path path)
{
  if (mImageViews.imageMonitor().containsRow(path, true)) {
    mParent->onError(PBDev::Error() << PB::ErrorCode::FolderAlreadyImported);
  }
  auto maybeError = mImportLogic.addImportFolder(path);

  if (maybeError) {
    mParent->onError(maybeError.value());
    return;
  }

  mImportLogic.start(path);
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

void Photobook::loadStagedImages()
{
  auto stagedImages =
      mProjectPersistence.currentProject()->active().stagedImages;

  std::vector<std::shared_ptr<VirtualImage>> stage;

  for (auto i = 0; i < stagedImages.size(); ++i) {
    if (std::filesystem::is_regular_file(stagedImages.at(i))) {

      auto regularImage = ImageFactory::inst().createImage(stagedImages.at(i));

      mImageViews.stagedImages().addPicture(regularImage);
    }
    else if (std::filesystem::is_directory(stagedImages.at(i))) {
      auto textImage = ImageFactory::inst().createImage(stagedImages.at(i));
      mImageViews.stagedImages().addPicture(textImage);
    }
    else {
      PBDev::basicAssert(false);
    }
  }
}
/*
void Photobook::update(PBDev::ObservableSubject &subject)
{
  if (dynamic_cast<PdfPoDoFoExport *>(&subject) != nullptr) {

    auto &pdfExporter = static_cast<PdfPoDoFoExport &>(subject);
    auto [progress, maxProgress] = pdfExporter.progress();
    mParent->onExportProgressUpdate(progress, maxProgress);
    if (progress == maxProgress) {
      mParent->onExportFinished();
    }
  }
  else {
    PBDev::basicAssert(false);
  }
}
*/
void Photobook::onError(PBDev::Error error) { mParent->onError(error); }

void Photobook::exportAlbum(std::string name, Path path)
{
  mExportLogic.start(Context::inst().sStopSource);

  auto              stagedPhotos = mImageViews.stagedImages().stagedPhotos();
  std::vector<Path> fullPaths;
  for (auto photo : stagedPhotos) {
    fullPaths.push_back(photo->frontend().full);
  }

  mExportFactory.configure(mProjectPersistence.currentProject());
  mExportFactory.configure(mPlatformInfo->localStatePath);
  mExporters.push_back(mExportFactory.makePdf(name, path, fullPaths));

  for (auto exporter : mExporters) {
    // exporter->attach(this);
  }

  mExportLogic.start(Context::inst().sStopSource);
}

ProjectPersistence &Photobook::project() { return mProjectPersistence; }

ImageViews &Photobook::imageViews() { return mImageViews; }

void Photobook::onProjectRead(
    std::vector<std::vector<std::shared_ptr<VirtualImage>>> &unstagedImages,
    std::vector<std::shared_ptr<VirtualImage>>              &stagedImages,
    std::vector<Path>                                       &roots)
{
  configure(mProjectPersistence.currentProject());
  mImageViews.imageMonitor().replaceImageMonitorData(unstagedImages, roots);
  mImageViews.stagedImages().configure(stagedImages);
  mParent->onProjectRead();
}

void Photobook::onMetadataUpdated() { mParent->onMetadataUpdated(); }

void Photobook::onPersistenceError(PBDev::Error error)
{
  mParent->onPersistenceError(error);
}

void Photobook::newProject(std::string name)
{
  // TODO: mProjectPersistence should announce mImportLogic when the project was
  // updated
  mProjectPersistence.newProject(name, std::make_shared<Project>());
  mImportLogic.configure(mProjectPersistence.currentProject());
  ImageFactory::inst().configure(mProjectPersistence.currentProject());
}

void Photobook::onMappingStarted(Path path) { mParent->onMappingStarted(path); }

void Photobook::onMappingAborted(Path path) { mParent->onMappingAborted(path); }

void Photobook::onMappingFinished(Path root, std::vector<Path> newFiles)
{
  std::vector<std::shared_ptr<VirtualImage>> imagesSet;

  std::vector<std::pair<Path, Path>> keyAndPaths;

  for (auto i = 0; i < newFiles.size(); ++i) {
    auto virtualImage = PB::ImageFactory::inst().createImage(newFiles.at(i));
    imagesSet.push_back(virtualImage);
    keyAndPaths.push_back({virtualImage->keyPath(), newFiles.at(i)});
  }

  mImageViews.imageMonitor().addRow(root, imagesSet);

  mParent->onMappingFinished(root);

  mImportLogic.processImages(root, keyAndPaths);
}

void Photobook::onImportStop(Path) {}

void Photobook::onImageProcessed(Path key, Path root, Path full, Path medium,
                                 Path small)
{
  mImageViews.imageMonitor().image(key)->setSizePath(full, medium, small);

  auto [progress, progressCap] = mImportLogic.imageProcessingProgress(root);
  auto [globalProgress, globalProgressCap] =
      mImportLogic.imageProcessingProgress();

  mParent->onProgressUpdate(globalProgress, (int)globalProgressCap);

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

void Photobook::onProjectRenamed() {}

} // namespace PB