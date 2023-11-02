#include <pb/Photobook.h>

#include <pb/RegularImage.h>
#include <pb/SerializationStrategy.h>
#include <pb/TextImage.h>

namespace PB {
Photobook::Photobook(PhotobookListener &listener, Path centralPersistencePath,
                     std::pair<int, int> screenSize)
    : mParent(listener), mCentralPersistencePath(centralPersistencePath),
      mCentralPersistence(mCentralPersistencePath),
      mThumbnailsProcessor(screenSize),
      mPaperSettings(Context::A4_LANDSCAPE_PAPER), mExportFactory()
{
  printDebug("Photobook created.\n");

  auto maybeError = mCentralPersistence.connect();
  if (maybeError) {
    onError(maybeError.value());
  }

  mImageSupport.setListener(mGallery.slot());
}

Photobook::~Photobook()
{
  printDebug("Photobook destructed.\n");
  mImageSupport.clear();
}

PaperSettings Photobook::paperSettings() const { return mPaperSettings; }

void Photobook::setPaperSettings(PaperSettings paperSettings)
{
  mPaperSettings = paperSettings;
}

void Photobook::configureProject(PB::Project project)
{
  mProject = project;

  mThumbnailsProcessor.provideProjectDetails(project.details());

  auto importedFolders = mProject.details().importedFolderList();
  for (auto &path : importedFolders) {
    addImportFolder(path);
  }
  auto stagedImages = mProject.details().stagedImagesList();

  for (auto i = 0; i < stagedImages.size(); ++i) {
    if (std::filesystem::is_regular_file(stagedImages.at(i))) {
      Thumbnails thumbnail(stagedImages.at(i));
      auto       regularImage = std::make_shared<RegularImage>(thumbnail);
      regularImage->setFullSizePath(stagedImages.at(i));

      mImageSupport.stagePhoto({regularImage});
      mParent.onStagedImageAdded({regularImage});
    }
    else if (std::filesystem::is_directory(stagedImages.at(i))) {
      auto textImage =
          std::make_shared<TextImage>(stagedImages.at(i).stem().string());
      textImage->setFullSizePath(stagedImages.at(i));

      mImageSupport.stagePhoto({textImage});
      mParent.onStagedImageAdded({textImage});
    }
    else {
      PB::basicAssert(false);
    }
  }
}
void Photobook::newEmptyProject() {}

void Photobook::addImportFolder(Path importPath)
{
  auto errorOrPath = FileInfo::validInputRootPath(importPath);
  if (std::holds_alternative<Error>(errorOrPath)) {
    mParent.onError(std::get<Error>(errorOrPath));
  }
  else {
    auto path = std::get<Path>(errorOrPath);
    printDebug("Add Input folder %s\n", path.string().c_str());

    auto &rootFolders = imageSupport().groups();
    if (rootFolders.find(path) != rootFolders.end()) {
      mParent.onError(Error() << ErrorCode::FolderAlreadyImported
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
    mParent.onExportProgressUpdate(progress, maxProgress);
    if (progress == maxProgress) {
      mParent.onExportFinished();
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

  mParent.onAddingUnstagedImagePlaceholder((unsigned)newMediaMap.size());

  mParent.onMappingFinished(rootPath);

  auto supportDirectoryPath = mProject.details().parentDirectory() /
                              mProject.details().supportDirName();
  if (!std::filesystem::exists(supportDirectoryPath)) {
    if (!FilePersistence::createDirectory(supportDirectoryPath)) {
      mParent.onFinished(rootPath);
      return;
    }
  }

  if (newMediaMap.empty()) {
    mParent.onFinished(rootPath);
  }

  auto start = std::chrono::high_resolution_clock::now();

  mProgress[rootPath] = 0;
  const int maxProgress = (int)newMediaMap.size();

  auto importFolderIndex = mImageSupport.groups().at(rootPath);

  auto groupIdentifier = std::to_string(importFolderIndex) + "sep";

  mThumbnailsProcessor.generateThumbnails(
      newMediaMap, groupIdentifier,
      [this, rootPath{rootPath}, start{start}, maxProgress{maxProgress}](
          Path input, Path smallOutput, Path mediumOutput, int position) {
        mParent.onProgressUpdate(rootPath, mProgress.at(rootPath),
                                 (int)maxProgress);

        mParent.onUnstagedImageAdded(rootPath, input, mediumOutput, smallOutput,
                                     position);

        mParent.post([this, rootPath{rootPath}, input{input},
                      smallOutput{smallOutput}, start{start},
                      mediumOutput{mediumOutput}, maxProgress{maxProgress}]() {
          mImageSupport.image<RegularImage>(input)
              ->buildSmallImage(smallOutput)
              .buildMediumImage(mediumOutput);
          mProgress[rootPath]++;
          if (mProgress.at(rootPath) == maxProgress) {

            auto end = std::chrono::high_resolution_clock::now();
            std::chrono::duration<double> duration = end - start;
            PB::printDebug("Duration: %f\n", duration.count());

            mParent.onFinished(rootPath);
            mMappingJobs.erase(rootPath);
          }
        });
      });
}

void Photobook::onError(Error error) { mParent.onError(error); }

Gallery &Photobook::gallery() { return mGallery; }

void Photobook::exportAlbum(std::string name, Path path)
{
  auto stagedPhotos = mImageSupport.stagedPhotosFullPaths();
  mExportFactory.updateConfiguration(mPaperSettings, mCentralPersistencePath);
  mExporters.push_back(mExportFactory.makePdf(name, path, stagedPhotos));

  for (auto exporter : mExporters) {
    exporter->attach(this);
    exporter->start();
  }
}

void Photobook::discardPhotobook()
{
  mThumbnailsProcessor.halt();
  PB::printDebug("Discard Photobook\n");
}

void Photobook::savePhotobook()
{
  savePhotobook(mProject.details().projectFile());
}

void Photobook::savePhotobook(Path newPath)
{
  bool newSaveFile = false;
  Path oldProjectFile = mProject.details().projectFile();
  Path oldSupportFolder = mProject.details().supportFolder();

  if (newPath != oldProjectFile) {
    mProject.updateProjectPath(newPath.parent_path());
    mProject.updateProjectName(newPath.stem().string());
    newSaveFile = true;

    std::pair<std::string, std::string> entry = {
        boost::uuids::to_string(mProject.details().uuid()),
        mProject.details().projectFile().string()};

    mCentralPersistence.write(
        entry, [this, newPath{Path(newPath)}](std::optional<Error> maybeError) {
          if (maybeError) {
            mParent.onError(Error() << ErrorCode::CannotSaveFile);
          }
        });
  }

  auto &groupsSet = imageSupport().groups();

  std::vector<Path> goups;
  for (auto &path : groupsSet) {
    goups.push_back(path.first);
  }
  mProject.details().setImportedPaths(goups);

  mProject.details().setPaperSettings(mPaperSettings);

  auto it = imageSupport().stagedIterator();

  mProject.details().setStagedImages(imageSupport().stagedPhotos());

  auto projectDetailsOrError =
      Text::serialize<ProjectDetails>(0, {"root", mProject.details()});

  if (std::holds_alternative<Error>(projectDetailsOrError)) {
    PB::basicAssert(false);
    return;
  }

  FilePersistence persistence(newPath);
  persistence.write(std::get<Json>(projectDetailsOrError).at("root"),
                    [this, oldProjectFile, oldSupportFolder,
                     newSaveFile](std::optional<Error> maybeError) {
                      if (maybeError) {
                        mParent.onError(maybeError.value());
                      }
                      else {
                        if (newSaveFile) {
                          std::filesystem::remove(oldSupportFolder);
                          std::filesystem::remove(oldProjectFile);
                        }
                      }
                    });

  PB::printDebug("Save Photobook %s\n", newPath.string().c_str());
}

ImageSupport &Photobook::imageSupport() { return mImageSupport; }

bool Photobook::projectDefaultSaved()
{
  auto projectParentPath = mProject.details().parentDirectory().string();

  if (projectParentPath.find(mCentralPersistencePath.string()) ==
      std::string::npos) {
    return false;
  }
  return true;
}
} // namespace PB