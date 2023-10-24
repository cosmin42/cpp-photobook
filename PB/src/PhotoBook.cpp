#include <pb/PhotoBook.h>


namespace PB {
PhotoBook::PhotoBook(PhotobookListener &listener, Path centralPersistencePath,
          std::pair<int, int> screenSize)
    : mParent(listener), mCentralPersistencePath(centralPersistencePath),
      mCentralPersistence(mCentralPersistencePath),
      mThumbnailsProcessor(screenSize), mPaperSettings(A4_PAPER)
{
  printDebug("Photobook created.\n");

  auto maybeError = mCentralPersistence.connect();
  if (maybeError) {
    onError(maybeError.value());
  }
}

PhotoBook::~PhotoBook()
{
  printDebug("Photobook destructed.\n");
  mImagePaths.clear();
}

PaperSettings PhotoBook::paperSettings() { return mPaperSettings; }

void PhotoBook::setPaperSettings(PaperSettings paperSettings)
{
  mPaperSettings = paperSettings;
}

void PhotoBook::loadProject(Path const                               &path,
                 std::function<void(std::optional<Error>)> onReturn)
{
  FilePersistence projectPersistence(path);
  projectPersistence.read([this,
                           onReturn](std::variant<Json, Error> jsonOrError) {
    if (std::holds_alternative<Error>(jsonOrError)) {
      auto error = std::get<Error>(jsonOrError);
      onReturn(error);
    }
    else {
      auto &json = std::get<Json>(jsonOrError);

      auto projectDetailsOrError = ProjectDetails::parse(json);

      if (std::holds_alternative<Error>(projectDetailsOrError)) {
        onReturn(std::get<Error>(projectDetailsOrError));
      }
      else {
        auto &projectDetails = std::get<ProjectDetails>(projectDetailsOrError);

        mProject = Project(projectDetails);

        mThumbnailsProcessor.provideProjectDetails(projectDetails);

        auto importedFolders = mProject.details().importedFolderList();
        for (auto &path : importedFolders) {
          addImportFolder(path);
        }
        auto stagedImages = mProject.details().stagedImagesList();

        for (auto i = 0; i < stagedImages.size(); ++i) {
          addStagedPhoto(Thumbnails(stagedImages.at(i)));
        }

        onReturn(std::nullopt);
      }
    }
  });
}

void PhotoBook::addImportFolder(Path importPath)
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

void PhotoBook::update(ObservableSubject &subject)
{
  auto &mediaMap = static_cast<MediaMapper &>(subject);
  if (mediaMap.state() == MediaMapState::Finished) {
    onImportFolderMapped(mediaMap.root(), mediaMap.importedDirectories());
  }
}

void PhotoBook::onImportFolderMapped(Path              rootPath,
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

void PhotoBook::onError(Error error) { mParent.onError(error); }

Gallery &PhotoBook::gallery() { return mGallery; }

std::optional<PB::Path> PhotoBook::selectedImportFolder()
{
  auto selectedIndex = gallery().selectedIndex();
  if (selectedIndex > -1) {
    return imageSupport().groupByIndex(selectedIndex);
  }
  else {
    return std::nullopt;
  }
}

auto PhotoBook::loadGalleryImage(std::string const &path, cv::Size size)
    -> std::shared_ptr<cv::Mat>
{
  auto image = mImageReader.read(path);
  return Process::resize(size, true)(image);
}

template <> Exporter<Pdf> &PhotoBook::exporter() { return mExporter; }

void PhotoBook::discardPhotoBook() { PB::printDebug("Discard Photobook\n"); }

void PhotoBook::savePhotoBook()
{
  savePhotoBook(mProject.details().projectFile());
}

void PhotoBook::savePhotoBook(Path newPath)
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

  auto it = imageSupport().stagedIterator();

  mProject.details().setStagedImages(imageSupport().stagedPhotos());

  auto projectDetailsJson = Json(mProject.details());

  FilePersistence persistence(newPath);

  persistence.write(projectDetailsJson,
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

void PhotoBook::addStagedPhoto(Thumbnails th)
{
  mImagePaths.stagePhoto(th);
  mParent.onStagedImageAdded(th);
}

void PhotoBook::deleteStagedPhoto(std::vector<int> positions)
{
  mImagePaths.unstagePhoto(positions);
  mParent.onStagedImageRemoved(positions);
}

void PhotoBook::insertStagedPhoto(Thumbnails path, int position)
{
  mImagePaths.stagePhoto(path, position);
}

void PhotoBook::removeStagedPhoto(int index)
{
  mImagePaths.unstagePhoto({index});
}

std::vector<Thumbnails> &PhotoBook::stagedPhotos()
{
  return mImagePaths.stagedPhotos();
}

ImageSupport &PhotoBook::imageSupport() { return mImagePaths; }

bool PhotoBook::projectDefaultSaved()
{
  auto projectParentPath = mProject.details().parentDirectory().string();

  if (projectParentPath.find(mCentralPersistencePath.string()) ==
      std::string::npos) {
    return false;
  }
  return true;
}
} // namespace PB