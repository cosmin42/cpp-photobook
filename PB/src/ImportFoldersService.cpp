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

  auto stopSource =
      mTaskCruncher->crunch("image-search-job", mSearches.at(jobId),
                            PBDev::ProgressJobName{"image-search"});

  UNUSED(stopSource);
}

std::unordered_map<PBDev::ImageId, GenericImagePtr, boost::hash<PBDev::ImageId>>
ImportFoldersService::createPlaceholders(std::vector<Path> searchResults)
{
  std::unordered_map<PBDev::ImageId, GenericImagePtr,
                     boost::hash<PBDev::ImageId>>
      placeholders;

  for (auto &path : searchResults) {
    PBDev::ImageId imageId(RuntimeUUID::newUUID());

    GenericImagePtr placeholder =
        std::make_shared<RegularImageV2>(RegularImageV2::defaultHash(), path);

    placeholders.emplace(imageId, placeholder);
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
    mSearches.erase(jobId);

    auto placeholders = createPlaceholders(searchResults);

    mListener->onSearchingFinished(root, placeholders);

    startThumbnailsCreation(jobId, placeholders);

    // TODO: See if this is still needed
    // std::vector<Path> onlyFilesResults(searchResults);
    // auto              it = std::remove_if(
    //    onlyFilesResults.begin(), onlyFilesResults.end(),
    //    [](Path path) { return std::filesystem::is_directory(path); });

    // onlyFilesResults.erase(it, onlyFilesResults.end());
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
  auto root = mRootPaths.at(jobId);

  if (mThumbnailsJobs.at(jobId).isFinished()) {
    mThumbnailsJobs.erase(jobId);
    mRootPaths.erase(jobId);
    mSearches.erase(jobId);
  }
  mListener->onImageProcessed(imageId, root, image);
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