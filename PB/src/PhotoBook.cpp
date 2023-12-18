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

  // mImageSupport.setListener(mGallery.slot());
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
      PB::basicAssert(false);
    }
  }

  mImageViews.stagedImages().addPictures(stage);
}

void Photobook::addImportFolder(Path path)
{
  auto maybeError = mImportLogic.addImportFolder(path);

  if (maybeError) {
    mParent->onError(maybeError.value());
  }
}

void Photobook::update(ObservableSubject &subject)
{
  if (dynamic_cast<PdfPoDoFoExport *>(&subject) != nullptr) {
    auto &pdfExporter = static_cast<PdfPoDoFoExport &>(subject);
    auto [progress, maxProgress] = pdfExporter.progress();
    mParent->onExportProgressUpdate(progress, maxProgress);
    if (progress == maxProgress) {
      mParent->onExportFinished();
    }
  }
  else
  {
    PB::basicAssert(false);
  }
}

void Photobook::onImportFolderMapped(Path              rootPath,
                                     std::vector<Path> newMediaMap)
{
  /*
  mImageSupport.addGroup(rootPath);
  std::vector<std::shared_ptr<VirtualImage>> imagesSet;
  for (auto p : newMediaMap) {
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
      PB::basicAssert(false);
    }
  }

  mImageViews.imageMonitor().addRow(rootPath, imagesSet);

  //mImageSupport.addImage(rootPath, newMediaMap, imagesSet);

  mParent->onAddingUnstagedImagePlaceholder((unsigned)newMediaMap.size());

  mParent->onMappingFinished(rootPath);

  if (newMediaMap.empty()) {
    mParent->onFinished(rootPath);
  }

  auto start = std::chrono::high_resolution_clock::now();

  mProgress[rootPath] = 0;
  const int maxProgress = (int)newMediaMap.size();

  auto importFolderIndex = mImageSupport.groupIndex(rootPath);

  auto groupIdentifier = std::to_string(importFolderIndex) + "sep";

  mThumbnailsProcessor.generateThumbnails(
      newMediaMap, groupIdentifier,
      [this, rootPath{rootPath}, start{start}, maxProgress{maxProgress}](
          Path input, Path smallOutput, Path mediumOutput, int position) {
        mParent->onProgressUpdate(rootPath, mProgress.at(rootPath),
                                  (int)maxProgress);

        mParent->onUnstagedImageAdded(rootPath, input, mediumOutput,
                                      smallOutput, position);

        mParent->post([this, rootPath{rootPath}, input{input},
                       smallOutput{smallOutput}, start{start},
                       mediumOutput{mediumOutput}, maxProgress{maxProgress}]() {
          auto image = mImageSupport.image<VirtualImage>(input);

          image->setMediumSizePath(mediumOutput);
          image->setSmallSizePath(smallOutput);

          mImageSupport.updateStagedPhoto(image);

          mProgress[rootPath]++;
          if (mProgress.at(rootPath) == maxProgress) {

            auto end = std::chrono::high_resolution_clock::now();
            std::chrono::duration<double> duration = end - start;
            PB::printDebug("Duration: %f\n", duration.count());

            mParent->onFinished(rootPath);
            mMappingJobs.erase(rootPath);
          }
        });
      });
      */
}

void Photobook::onError(Error error) { mParent->onError(error); }

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

void Photobook::discardPhotobook()
{
  mImportLogic.stopAll();
}

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

  if (std::holds_alternative<Error>(projectDetailsOrError)) {
    PB::basicAssert(false);
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

void Photobook::onProjectRead(Project project) { configureProject(project); }

void Photobook::onMetadataRead(ProjectMetadata projectMetadata) {}

void Photobook::onMetadataRead(std::vector<ProjectMetadata> projectMetadata) {}

void Photobook::onMetadataPersistenceError(Error error) { onError(error); }

void Photobook::onProjectPersistenceError(Error error) { onError(error); }

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

} // namespace PB