#include <pb/Photobook.h>

#include <pb/FilePersistence.h>
#include <pb/RegularImage.h>
#include <pb/SerializationStrategy.h>
#include <pb/TextImage.h>

namespace PB {
Photobook::Photobook(Path applicationLocalStatePath)
    : mApplicationLocalStatePath(applicationLocalStatePath),
      mThumbnailsProcessor(), mExportFactory()
{
  printDebug("Photobook created.\n");

  mPersistence =
      std::make_shared<PB::Persistence>(applicationLocalStatePath, this, this);

  mImageSupport.setListener(mGallery.slot());
}

Photobook::~Photobook()
{
  printDebug("Photobook destructed.\n");
  mImageSupport.clear();
}

void Photobook::configure(std::pair<int, int> screenSize)
{
  mThumbnailsProcessor.setScreenSize(screenSize);
}

void Photobook::configure(std::shared_ptr<PhotobookListener> listener)
{
  mParent = listener;
}

void Photobook::configureProject(PB::Project project)
{
  mProject = project;

  mThumbnailsProcessor.provideProjectDetails(project.active());
}

void Photobook::loadProject() {
  auto importedFolders = mProject.active().importedFolderList();
  for (auto &path : importedFolders) {
    addImportFolder(path);
  }

  auto stagedImages = mProject.active().stagedImagesList();

  for (auto i = 0; i < stagedImages.size(); ++i) {
    if (std::filesystem::is_regular_file(stagedImages.at(i))) {
      Thumbnails thumbnail(stagedImages.at(i));
      auto       regularImage = std::make_shared<RegularImage>(thumbnail);
      regularImage->setFullSizePath(stagedImages.at(i));

      mImageSupport.stagePhoto({regularImage});
      mParent->onStagedImageAdded({regularImage});
    }
    else if (std::filesystem::is_directory(stagedImages.at(i))) {
      auto textImage =
          std::make_shared<TextImage>(stagedImages.at(i).stem().string());
      textImage->setFullSizePath(stagedImages.at(i));

      mImageSupport.stagePhoto({textImage});
      mParent->onStagedImageAdded({textImage});
    }
    else {
      PB::basicAssert(false);
    }
  }
}

void Photobook::addImportFolder(Path importPath)
{
  auto errorOrPath = FileInfo::validInputRootPath(importPath);
  if (std::holds_alternative<Error>(errorOrPath)) {
    mParent->onError(std::get<Error>(errorOrPath));
  }
  else {
    auto path = std::get<Path>(errorOrPath);
    printDebug("Add Input folder %s\n", path.string().c_str());

    if (imageSupport().containsGroup(path)) {
      mParent->onError(Error() << ErrorCode::FolderAlreadyImported
                               << "Folder already imported.");
      return;
    }

    setChangeFunction([this](PB::Observer *current, PB::Observer *other) {
      if (other) {
        for (auto &[key, value] : mMappingJobs) {
          value.dettach(current);
          value.attach(other);
        }

        for (auto &exporter : mExporters) {
          exporter->dettach(current);
          exporter->attach(other);
        }
      }
      else {
        PB::basicAssert(false);
      }
    });
    auto mapper = MediaMapper(path);
    mapper.attach(this);
    mMappingJobs.emplace(path, mapper);
    mMappingJobs.at(importPath).start();
  }
}

void Photobook::update(ObservableSubject &subject)
{
  if (dynamic_cast<MediaMapper *>(&subject) != nullptr) {
    auto &mediaMap = static_cast<MediaMapper &>(subject);
    if (mediaMap.state() == MediaMapState::Finished) {
      onImportFolderMapped(mediaMap.root(), mediaMap.importedDirectories());
    }
  }
  else if (dynamic_cast<PdfPoDoFoExport *>(&subject) != nullptr) {
    auto &pdfExporter = static_cast<PdfPoDoFoExport &>(subject);
    auto [progress, maxProgress] = pdfExporter.progress();
    mParent->onExportProgressUpdate(progress, maxProgress);
    if (progress == maxProgress) {
      mParent->onExportFinished();
    }
  }
}

void Photobook::onImportFolderMapped(Path              rootPath,
                                     std::vector<Path> newMediaMap)
{
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

  mImageSupport.addImage(rootPath, newMediaMap, imagesSet);

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
}

void Photobook::onError(Error error) { mParent->onError(error); }

Gallery &Photobook::gallery() { return mGallery; }

void Photobook::exportAlbum(std::string name, Path path)
{
  auto              stagedPhotos = mImageSupport.stagedPhotos();
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
  mThumbnailsProcessor.halt();
  PB::printDebug("Discard Photobook\n");
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

  mProject.active().setImportedPaths(imageSupport().groups());
  mProject.active().setPaperSettings(mProject.active().paperSettings());
  mProject.active().setStagedImages(imageSupport().stagedPhotos());

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

ImageSupport &Photobook::imageSupport() { return mImageSupport; }

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