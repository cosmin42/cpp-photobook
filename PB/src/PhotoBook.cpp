#include <pb/Photobook.h>

#include <pb/SerializationStrategy.h>

namespace PB {
Photobook::Photobook(PhotobookListener &listener, Path centralPersistencePath,
                     std::pair<int, int> screenSize)
    : mParent(listener), mCentralPersistencePath(centralPersistencePath),
      mCentralPersistence(mCentralPersistencePath),
      mThumbnailsProcessor(screenSize), mExporter(),
      mPaperSettings(Context::A4_PAPER)
{
  printDebug("Photobook created.\n");

  auto maybeError = mCentralPersistence.connect();
  if (maybeError) {
    onError(maybeError.value());
  }

  mImagePaths.setListener(mGallery.slot());
}

Photobook::~Photobook()
{
  printDebug("Photobook destructed.\n");
  mImagePaths.clear();
}

PaperSettings Photobook::paperSettings() { return mPaperSettings; }

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
    addStagedPhoto({Thumbnails(stagedImages.at(i))});
    mParent.onStagedImageAdded({Thumbnails(stagedImages.at(i))});
  }
}

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
      }
      else {
        for (auto &[key, value] : mMappingJobs) {
          value.dettach(current);
        }
      }
    });
    auto mapper = MediaMapper(path, this);
    mMappingJobs.emplace(path, mapper);
    mMappingJobs.at(importPath).start();
  }
}

void Photobook::update(ObservableSubject &subject)
{
  auto &mediaMap = static_cast<MediaMapper &>(subject);
  if (mediaMap.state() == MediaMapState::Finished) {
    onImportFolderMapped(mediaMap.root(), mediaMap.importedDirectories());
  }
}

void Photobook::onImportFolderMapped(Path              rootPath,
                                     std::vector<Path> newMediaMap)
{
  mImagePaths.addGroup(rootPath);
  mImagePaths.addFullPaths(rootPath, newMediaMap);

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

  auto importFolderIndex = mImagePaths.groups().at(rootPath);

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
          mImagePaths.addSmallPath(input, smallOutput);
          mImagePaths.addMediumPath(input, mediumOutput);
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

std::optional<PB::Path> Photobook::selectedImportFolder()
{
  auto selectedIndex = gallery().selectedIndex();
  if (selectedIndex > -1) {
    return imageSupport().groupByIndex(selectedIndex);
  }
  else {
    return std::nullopt;
  }
}

auto Photobook::loadGalleryImage(std::string const &path, cv::Size size)
    -> std::shared_ptr<cv::Mat>
{
  auto image = mImageReader.read(path);
  return Process::resize(size, true)(image);
}

template <> Exporter<Pdf> &Photobook::exporter() { return mExporter; }

void Photobook::discardPhotobook() { PB::printDebug("Discard Photobook\n"); }

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

void Photobook::addStagedPhoto(std::vector<Thumbnails> photos, int position)
{
  mImagePaths.stagePhoto(photos, position);
}

void Photobook::deleteStagedPhoto(std::vector<int> positions)
{
  mImagePaths.unstagePhoto(positions);
}

void Photobook::removeStagedPhoto(int index)
{
  mImagePaths.unstagePhoto({index});
}

std::vector<Thumbnails> &Photobook::stagedPhotos()
{
  return mImagePaths.stagedPhotos();
}

ImageSupport &Photobook::imageSupport() { return mImagePaths; }

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