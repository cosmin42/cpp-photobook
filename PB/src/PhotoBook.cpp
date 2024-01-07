#include <pb/PhotoBook.h>

#include <pb/image/Image.h>

namespace PB {
Photobook::Photobook(Path localStatePath, Path installationPath)
    : mPlatformInfo(
          std::make_shared<PlatformInfo>(installationPath, localStatePath)),
      mPersistence(localStatePath, this, this)
{
  VirtualImage::platformInfo = mPlatformInfo;
  ProjectSnapshot::platformInfo = mPlatformInfo;

  mImportLogic.configure((ImportFoldersLogicListener *)this);
  mImportLogic.configure((ThreadScheduler *)this);
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

void Photobook::configure(DashboardListener *listener)
{
  mDashboardListener = listener;
}

void Photobook::configure(std::shared_ptr<PB::Project> project)
{
  mProject = project;

  mImportLogic.configure(mProject);
}

void Photobook::renameProject(std::string uuid, std::string oldName,
                              std::string newName)
{
  Path oldProjectPath = VirtualImage::platformInfo->localStatePath /
                        (oldName + Context::BOOK_EXTENSION);
  Path newProjectPath = VirtualImage::platformInfo->localStatePath /
                        (newName + Context::BOOK_EXTENSION);

  PB::ProjectMetadata projectMetadata(uuid, newProjectPath.string());

  if (mProject) {
    mProject->active().name = newName;
    mProject->sync();

    mPersistence.persistProject(newName, mProject->active());
  }
  else
  {
  }
  mPersistence.persistMetadata(projectMetadata);
  if (newName != oldName) {
    std::filesystem::rename(oldProjectPath, newProjectPath);
  }
}

void Photobook::loadProject()
{
  for (auto &path : mProject->active().importedPaths) {
    addImportFolder(path);
  }

  loadStagedImages();
}

void Photobook::unloadProject()
{
  mImportLogic.stopAll();
  if (mImportLogic.runningImageProcessingJobs().empty()) {
    mProject = nullptr;
  }
  else {
    mMarkProjectForDeletion = true;
  }
}

void Photobook::recallMetadata() { mPersistence.recallMetadata(); }

void Photobook::recallProject(Path path) { mPersistence.recallProject(path); }

void Photobook::deleteProject(std::string id)
{
  Path projectFile = mProject->metadata().projectFile();
  mPersistence.deleteMetadata(id);
  mPersistence.deleteProject(projectFile);
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
  auto stagedImages = mProject->active().stagedImages;

  std::vector<std::shared_ptr<VirtualImage>> stage;

  for (auto i = 0; i < stagedImages.size(); ++i) {
    if (std::filesystem::is_regular_file(stagedImages.at(i))) {

      auto regularImage = std::make_shared<RegularImage>(stagedImages.at(i));
      regularImage->setFullSizePath(stagedImages.at(i));

      mImageViews.stagedImages().addPicture(regularImage);
    }
    else if (std::filesystem::is_directory(stagedImages.at(i))) {
      auto textImage = std::make_shared<TextImage>(
          stagedImages.at(i).stem().string(), stagedImages.at(i));
      textImage->setFullSizePath(stagedImages.at(i));

      mImageViews.stagedImages().addPicture(textImage);
    }
    else {
      PBDev::basicAssert(false);
    }
  }
}

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

void Photobook::onError(PBDev::Error error) { mParent->onError(error); }

void Photobook::exportAlbum(std::string name, Path path)
{
  auto              stagedPhotos = mImageViews.stagedImages().stagedPhotos();
  std::vector<Path> fullPaths;
  for (auto photo : stagedPhotos) {
    fullPaths.push_back(photo->resources().full);
  }

  mExportFactory.updateConfiguration(mProject->active().paperSettings,
                                     mPlatformInfo->localStatePath);
  mExporters.push_back(mExportFactory.makePdf(name, path, fullPaths));

  for (auto exporter : mExporters) {
    exporter->attach(this);
  }
}

ProjectSnapshot &Photobook::activeProject() { return mProject->active(); }

void Photobook::saveProject() { saveProject(mProject->cache().name); }

void Photobook::saveProject(std::string name)
{
  auto oldName = mProject->cache().name;

  mProject->sync();

  Path projectPath = VirtualImage::platformInfo->localStatePath /
                     (name + Context::BOOK_EXTENSION);

  auto uuidStr = boost::uuids::to_string(mProject->active().uuid);
  auto fullPath = mProject->metadata().projectFile();
  PB::ProjectMetadata projectMetadata(uuidStr, projectPath.string());

  mPersistence.persistProject(name, mProject->active());
  mPersistence.persistMetadata(projectMetadata);

  if (name != oldName) {
    Path oldProjectPath = VirtualImage::platformInfo->localStatePath /
                          (oldName + Context::BOOK_EXTENSION);
    mPersistence.deleteProject(oldProjectPath);
  }
}

ImageViews &Photobook::imageViews() { return mImageViews; }

void Photobook::onProjectRead(std::shared_ptr<Project> project)
{
  configure(project);
  if (mDashboardListener) {
    mDashboardListener->onProjectRead();
  }
}

void Photobook::onMetadataRead(ProjectMetadata projectMetadata) {}

void Photobook::onMetadataRead(std::vector<ProjectMetadata> projectMetadata)
{
  mDashboardListener->onProjectsMetadataLoaded(projectMetadata);
}

void Photobook::onMetadataPersistenceError(PBDev::Error error)
{
  onError(error);
}

void Photobook::onProjectPersistenceError(PBDev::Error error)
{
  onError(error);
}

void Photobook::newProject(std::string name)
{
  configure(std::make_shared<Project>(name));

  saveProject();
}

void Photobook::onMappingStarted(Path path) { mParent->onMappingStarted(path); }

void Photobook::onMappingAborted(Path path) { mParent->onMappingAborted(path); }

void Photobook::onMappingFinished(Path root, std::vector<Path> newFiles)
{
  std::vector<std::shared_ptr<VirtualImage>> imagesSet;

  for (auto path : newFiles) {
    auto virtualImage = PB::ImageFactory::createImage(path);
    imagesSet.push_back(virtualImage);
  }

  mImageViews.imageMonitor().addRow(root, imagesSet);

  mParent->onMappingFinished(root);

  mImportLogic.processImages(root, newFiles);
}

void Photobook::onImportStop(Path) {}

void Photobook::onImageProcessed(Path root, Path full, Path medium, Path small)
{
  mImageViews.imageMonitor().image(full)->setSizePath(full, medium, small);

  auto [progress, progressCap] = mImportLogic.imageProcessingProgress(root);
  auto [globalProgress, globalProgressCap] =
      mImportLogic.imageProcessingProgress();

  mParent->onProgressUpdate(globalProgress, (int)globalProgressCap);

  auto [row, index] = mImageViews.imageMonitor().position(full);

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
      mProject = nullptr;
      mMarkProjectForDeletion = false;
    }
  }
}

void Photobook::onImageProcessingJobEnded(Path root) {}

void Photobook::post(std::function<void()> f) { mParent->post(f); }

std::vector<Path> Photobook::pendingMappingPathList() const
{
  return mImportLogic.pendingMappingFolders();
}

} // namespace PB