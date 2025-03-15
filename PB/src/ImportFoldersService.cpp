#include <pb/services/ImportFoldersService.h>

#include <pb/infra/FileInfo.h>

namespace PB::Service {

void ImportFoldersService::addImportFolder(Path path)
{
  auto maybeProject = mProjectManagementService->maybeLoadedProjectInfo();
  PBDev::basicAssert(maybeProject != nullptr);
  if (maybeProject->second.imageMonitor()->containsRow(path, true)) {
    mListener->onImportError(PBDev::Error() << PB::ErrorCode::FolderAlreadyImported);
    return;
  }

  auto errorOrPath = PBDev::FileInfo::validInputRootPath(path);
  if (std::holds_alternative<PBDev::Error>(errorOrPath)) {
    mListener->onImportError(std::get<PBDev::Error>(errorOrPath));
    return;
  }

  for (auto &[key, value] : mRootPaths) {
    if (PBDev::FileInfo::contains(value, path)) {
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

  auto stopSource = mTaskCruncher->crunch("image-search-job", mSearches.at(jobId),
                        PBDev::ProgressJobName{"image-search"});

  UNUSED(stopSource);
}

void ImportFoldersService::startThumbnailsCreation(
    PBDev::ThumbnailsJobId jobId, std::vector<Path> searchResults)
{
  mThumbnailsJobs.emplace(jobId, ThumbnailsJob(jobId, searchResults));

  mThumbnailsJobs.at(jobId).configureListener(this);
  mThumbnailsJobs.at(jobId).configurePlatformInfo(mPlatformInfo);
  mThumbnailsJobs.at(jobId).configureProjectManagementService(
      mProjectManagementService);

  auto stopSource = mTaskCruncher->crunch("thumbnails-job", mThumbnailsJobs.at(jobId),
                        PBDev::ProgressJobName{"thumbnails"});

  UNUSED(stopSource);
}

void ImportFoldersService::onPicturesSearchFinished(
    PBDev::ThumbnailsJobId jobId, Path root, std::vector<Path> searchResults)
{
  // Improve this to take into consideration embedded empty folders
  if (searchResults.empty()) {
    mScheduler->post([this, root{root}]() {
      mListener->onImportError(PBDev::Error() << PB::ErrorCode::NoImages);
    });
  }
  else {
    mScheduler->post(
        [this, root{root}, searchResults{searchResults}, jobId{jobId}]() {
          mSearches.erase(jobId);
          mListener->onMappingFinished(root, searchResults);

          std::vector<Path> onlyFilesResults(searchResults);
          auto              it = std::remove_if(
              onlyFilesResults.begin(), onlyFilesResults.end(),
              [](Path path) { return std::filesystem::is_directory(path); });

          onlyFilesResults.erase(it, onlyFilesResults.end());

          startThumbnailsCreation(jobId, onlyFilesResults);
        });
  }
}

void ImportFoldersService::onPicturesSearchAborted(Path root) {}

void ImportFoldersService::imageProcessed(PBDev::ThumbnailsJobId jobId,
                                          GenericImagePtr        image)
{
  auto root = mRootPaths.at(jobId);

  mScheduler->post([this, root{root}, image{image}, jobId{jobId}]() {
    if (mThumbnailsJobs.at(jobId).isFinished()) {
      mThumbnailsJobs.erase(jobId);
      mRootPaths.erase(jobId);
      mSearches.erase(jobId);
    }
    mListener->onImageProcessed(root, root, image);
  });
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