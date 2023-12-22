#include <pb/Photobook.h>

#include <pb/FilePersistence.h>
#include <pb/RegularImage.h>
#include <pb/SerializationStrategy.h>
#include <pb/TextImage.h>

namespace PB {
Photobook::Photobook(Path applicationLocalStatePath)
    : mApplicationLocalStatePath(applicationLocalStatePath)
{
  mPersistence =
      std::make_shared<PB::Persistence>(applicationLocalStatePath, this, this);
}

void Photobook::configure(std::pair<int, int> screenSize)
{
  mImportLogic.configure(screenSize);
}

void Photobook::configure(std::shared_ptr<PhotobookListener> listener)
{
  mParent = listener;
}
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

void Photobook::configureProject(PB::Project project)
{
  mProject = project;

  mImportLogic.configure(project.active());
}

void Photobook::loadProject()
{
  auto importedFolders = mProject.active().importedFolderList();
  for (auto &path : importedFolders) {
    addImportFolder(path);
  }

  auto stagedImages = mProject.active().stagedImagesList();

  std::vector<std::shared_ptr<VirtualImage>> stage;

  for (auto i = 0; i < stagedImages.size(); ++i) {
    if (std::filesystem::is_regular_file(stagedImages.at(i))) {
      Thumbnails thumbnail(stagedImages.at(i));
      auto       regularImage = std::make_shared<RegularImage>(thumbnail);
      regularImage->setFullSizePath(stagedImages.at(i));

      stage.push_back(regularImage);
      // mParent->onStagedImageAdded({regularImage});
    }
    else if (std::filesystem::is_directory(stagedImages.at(i))) {
      auto textImage =
          std::make_shared<TextImage>(stagedImages.at(i).stem().string());
      textImage->setFullSizePath(stagedImages.at(i));

      // mImageSupport.stagePhoto({textImage});
      stage.push_back(textImage);
      // mParent->onStagedImageAdded({textImage});
    }
    else {
      PBDev::basicAssert(false);
    }
  }

  mImageViews.stagedImages().addPictures(stage);
}

void Photobook::recallMetadata() { mPersistence->recallMetadata(); }

void Photobook::recallProject(Path path) { mPersistence->recallProject(path); }

void Photobook::deleteProject(std::string id)
{
  mPersistence->deleteMetadata(id);
}

void Photobook::addImportFolder(Path path)
{
  auto maybeError = mImportLogic.addImportFolder(path);

  if (maybeError) {
    mParent->onError(maybeError.value());
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

void Photobook::onImportFolderMapped(Path              rootPath,
                                     std::vector<Path> newMediaMap)
{
}

void Photobook::onError(PBDev::Error error) { mParent->onError(error); }

void Photobook::exportAlbum(std::string name, Path path)
{
  auto              stagedPhotos = mImageViews.stagedImages().stagedPhotos();
  std::vector<Path> fullPaths;
  for (auto photo : stagedPhotos) {
    fullPaths.push_back(photo->resources().full);
  }

  mExportFactory.updateConfiguration(mProject.active().paperSettings(),
                                     mApplicationLocalStatePath);
  mExporters.push_back(mExportFactory.makePdf(name, path, fullPaths));

  for (auto exporter : mExporters) {
    exporter->attach(this);
    exporter->start();
  }
}

ProjectSnapshot &Photobook::activeProject() { return mProject.active(); }

void Photobook::discardPhotobook() { mImportLogic.stopAll(); }

void Photobook::savePhotobook()
{
  savePhotobook(mProject.metadata().projectFile());
}

void Photobook::savePhotobook(Path newPath)
{
  bool newSaveFile = false;
  Path oldProjectFile = mProject.metadata().projectFile();
  Path oldSupportFolder = mProject.active().supportFolder();

  if (newPath != oldProjectFile) {
    mProject.updateProjectPath(newPath.parent_path());
    mProject.updateProjectName(newPath.stem().string());
    newSaveFile = true;

    mPersistence->persistMetadata(mProject.metadata());
  }

  mProject.active().setImportedPaths(mImageViews.imageMonitor().rowList());
  mProject.active().setPaperSettings(mProject.active().paperSettings());
  mProject.active().setStagedImages(mImageViews.stagedImages().stagedPhotos());

  auto projectDetailsOrError =
      Text::serialize<ProjectSnapshot>(0, {"root", mProject.active()});

  if (std::holds_alternative<PBDev::Error>(projectDetailsOrError)) {
    PBDev::basicAssert(false);
    return;
  }

  mPersistence->persistProject(newPath, mProject.active());
  if (newSaveFile) {
    std::filesystem::remove(oldSupportFolder);
    std::filesystem::remove(oldProjectFile);
  }

  PB::printDebug("Save Photobook %s\n", newPath.string().c_str());
}

ProjectSnapshot Photobook::projectDetails() { return mProject.active(); }

ImageViews &Photobook::imageViews() { return mImageViews; }

bool Photobook::projectDefaultSaved()
{
  auto projectParentPath = mProject.active().parentDirectory().string();

  if (projectParentPath.find(mApplicationLocalStatePath.string()) ==
      std::string::npos) {
    return false;
  }
  return true;
}

void Photobook::onProjectRead(Project project)
{
  configureProject(project);
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

void Photobook::newProject()
{
  mProject = Project(mApplicationLocalStatePath);

  mPersistence->persistProject(mProject.metadata().projectFile(),
                               mProject.active());

  auto uuidStr = boost::uuids::to_string(mProject.active().uuid());
  auto fullPath = mProject.metadata().projectFile();
  PB::ProjectMetadata projectMetadata(uuidStr, fullPath.string());
  mPersistence->persistMetadata(projectMetadata);
}

void Photobook::onMapped(Path root, std::vector<Path> newFolders)
{
  std::vector<std::shared_ptr<VirtualImage>> imagesSet;

  for (auto p : newFolders) {
    if (std::filesystem::is_regular_file(p)) {
      auto newRegularImage = std::make_shared<RegularImage>(Thumbnails(p));
      newRegularImage->setFullSizePath(p);
      imagesSet.push_back(newRegularImage);
    }
    else if (std::filesystem::is_directory(p)) {
      auto textImage = std::make_shared<TextImage>(p.stem().string());
      textImage->setFullSizePath(p);
      imagesSet.push_back(textImage);
    }
    else {
      PBDev::basicAssert(false);
    }
  }

  mImageViews.imageMonitor().addRow(root, imagesSet);
}

void Photobook::onImportStop(Path) {}

void Photobook::onImageProcessed(Path root, Path full, Path medium, Path small,
                                 int progress, int progressCap)
{
  mProgress[root]++;
  mParent->onProgressUpdate(root, mProgress[root], (int)progressCap);

  auto rowIndex = mImageViews.imageMonitor().rowIndex(root);
  mImageViews.imageMonitor().image(full)->setSizePath(full, medium, small);

  if (mProgress[root] == progressCap) {
    mImageViews.imageMonitor().completeRow(rowIndex);

    mImportLogic.clearJob(root);
  }
}

void Photobook::onMappingAborted(Path) {}

void Photobook::post(std::function<void()> f) { mParent->post(f); }

} // namespace PB