#include <pb/services/ImportFoldersService.h>

#include <pb/infra/FileSupport.h>

namespace PB::Service {

void ImportFoldersService::addImportFolder(Path path)
{
  auto importedFoldersPaths = mProject->value.imageMonitor()->rowList();

  if (std::find(importedFoldersPaths.begin(), importedFoldersPaths.end(),
                path) != importedFoldersPaths.end()) {
    mListener->onImportError(PBDev::Error()
                             << PB::ErrorCode::FolderAlreadyImported);
    return;
  }

  auto isValid = infra::isValidMediaFolder(path);
  if (!isValid) {
    mListener->onImportError(PBDev::Error() << PB::ErrorCode::NotADirectory);
    return;
  }

  for (auto &[key, value] : mRootPaths) {
    if (value == path) {
      mListener->onImportError(PBDev::Error()
                               << PB::ErrorCode::FolderAlreadyImported);
      return;
    }
  }

  PBDev::ThumbnailsJobId jobId(RuntimeUUID::newUUID());
  mRootPaths[jobId] = path;
  mSearches.emplace(jobId, PicturesSearchJob(jobId, path));
  mSearches.at(jobId).setPicturesSearchJobListener(this);
  mSearches.at(jobId).assignUuid(
      PBDev::MapReducerTaskId(RuntimeUUID::newUUID()));
  mSearches.at(jobId).configureNoirMonitor(mNoirMonitor);

  auto stopSource =
      mTaskCruncher->crunch("image-search-job", mSearches.at(jobId),
                            PBDev::ProgressJobName{"image-search"});

  UNUSED(stopSource);
}

// TODO: check for duplicates names
void ImportFoldersService::importImagesByIndiviaulPaths(std::vector<Path> paths)
{
  PBDev::ThumbnailsJobId jobId(RuntimeUUID::newUUID());

  std::random_device              rd;
  std::mt19937                    gen(rd());
  std::uniform_int_distribution<> dis(
      0, OneConfig::RANDOM_FOLDERS_NAMES.size() - 1);
  int   randomIndex = dis(gen);
  auto &randomName = OneConfig::RANDOM_FOLDERS_NAMES[randomIndex];

  mRootPaths[jobId] = randomName;

  onPicturesSearchFinished(jobId, randomName, paths);
}

std::unordered_map<PBDev::ImageId, GenericImagePtr, boost::hash<PBDev::ImageId>>
ImportFoldersService::createPlaceholders(std::vector<Path> searchResults)
{
  std::unordered_map<PBDev::ImageId, GenericImagePtr,
                     boost::hash<PBDev::ImageId>>
      placeholders;

  for (auto &path : searchResults) {
    PBDev::ImageId imageId(RuntimeUUID::newUUID());

    GenericImagePtr placeholder;
    if (std::filesystem::is_directory(path)) {
      placeholder = std::make_shared<TextImageV2>(TextImageV2::defaultHash(),
                                                  path.stem().string());
    }
    else if (std::filesystem::is_regular_file(path)) {
      placeholder =
          std::make_shared<RegularImageV2>(RegularImageV2::defaultHash(), path);
    }
    else {
      PBDev::basicAssert(false);
    }
    placeholders[imageId] = placeholder;
  }

  return placeholders;
}

void ImportFoldersService::onPicturesSearchFinished(
    PBDev::ThumbnailsJobId jobId, Path root, std::vector<Path> searchResults)
{
  // Improve this to take into consideration embedded empty folders
  if (searchResults.empty()) {
    mListener->onImportError(PBDev::Error() << PB::ErrorCode::NoImages);
  }
  else {
    if (mSearches.contains(jobId)) {
      mSearches.erase(jobId);
    }
    mNoirMonitor->start("Placeholders", boost::uuids::to_string(jobId.raw()));
    auto placeholders = createPlaceholders(searchResults);
    mNoirMonitor->stop("Placeholders", boost::uuids::to_string(jobId.raw()));
    mListener->onSearchingFinished(root, placeholders);
    startThumbnailsCreation(jobId, placeholders);
  }
}

void ImportFoldersService::startThumbnailsCreation(
    PBDev::ThumbnailsJobId jobId,
    std::unordered_map<PBDev::ImageId, GenericImagePtr,
                       boost::hash<PBDev::ImageId>>
        placeholders)
{
  mThumbnailsJobs.emplace(jobId, ThumbnailsJob(jobId, placeholders));

  mThumbnailsJobs.at(jobId).configureListener(this);
  mThumbnailsJobs.at(jobId).configurePlatformInfo(mPlatformInfo);
  mThumbnailsJobs.at(jobId).configureProject(mProject);
  mThumbnailsJobs.at(jobId).configureNoirMonitor(mNoirMonitor);

  auto stopSource =
      mTaskCruncher->crunch("thumbnails-job", mThumbnailsJobs.at(jobId),
                            PBDev::ProgressJobName{"thumbnails"});

  UNUSED(stopSource);
}

void ImportFoldersService::onPicturesSearchAborted(Path root) {}

void ImportFoldersService::imageProcessed(PBDev::ThumbnailsJobId jobId,
                                          PBDev::ImageId         imageId,
                                          GenericImagePtr        image)
{
  auto &root = mRootPaths.at(jobId);

  mListener->onImageProcessed(imageId, root, image);
}

void ImportFoldersService::taskEnded()
{
  // TODO: Do the mThumbnailsJobs cleanup
  for (auto &[key, value] : mThumbnailsJobs) {
    if (value.isFinished()) {
      mScheduler->post([this, key]() {
        mRootPaths.erase(key);
        mSearches.erase(key);
        mThumbnailsJobs.erase(key);
      });
    }
  }
}

bool ImportFoldersService::isFinished(Path path)
{
  for (auto &[key, value] : mRootPaths) {
    if (value == path) {
      return false;
    }
  }
  return true;
}

} // namespace PB::Service
