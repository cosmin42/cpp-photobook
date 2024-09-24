#include <pb/ImportFoldersService.h>

#include <pb/util/FileInfo.h>

namespace PB {

std::optional<PBDev::Error> ImportFoldersService::addImportFolder(Path path)
{
  auto errorOrPath = PBDev::FileInfo::validInputRootPath(path);
  if (std::holds_alternative<PBDev::Error>(errorOrPath)) {
    return std::get<PBDev::Error>(errorOrPath);
  }

  for (auto &[key, value] : mRootPaths) {
    if (PBDev::FileInfo::contains(value, path)) {
      return PBDev::Error() << PB::ErrorCode::FolderAlreadyImported;
    }
  }

  PBDev::ThumbnailsJobId jobId(RuntimeUUID::newUUID());
  mRootPaths[jobId] = path;
  mSearches.emplace(jobId, PicturesSearchConfig(jobId, path));
  mSearches.at(jobId).setPicturesSearchConfigListener(this);
  mSearches.at(jobId).assignUuid(
      PBDev::MapReducerTaskId(RuntimeUUID::newUUID()));

  mTaskCruncher->crunch("image-search-job", mSearches.at(jobId),
                        PBDev::ProgressJobName{"image-search"});

  return std::nullopt;
}

void ImportFoldersService::startThumbnailsCreation(
    PBDev::ThumbnailsJobId jobId, std::vector<Path> searchResults)
{
  mThumbnailsJobs.emplace(jobId, ThumbnailsJob(jobId, searchResults));

  mThumbnailsJobs.at(jobId).configureListener(this);
  mThumbnailsJobs.at(jobId).configurePlatformInfo(mPlatformInfo);
  mThumbnailsJobs.at(jobId).configureProjectManagementSystem(
      mProjectManagementSystem);

  mTaskCruncher->crunch("thumbnails-job", mThumbnailsJobs.at(jobId),
                        PBDev::ProgressJobName{"thumbnails"});
}

void ImportFoldersService::onPicturesSearchFinished(
    PBDev::ThumbnailsJobId jobId, Path root, std::vector<Path> searchResults)
{
  mScheduler->post(
      [this, root{root}, searchResults{searchResults}, jobId{jobId}]() {
        mSearches.erase(jobId);
        mListener->onMappingFinished(root, searchResults);

        startThumbnailsCreation(jobId, searchResults);
      });
}

void ImportFoldersService::onPicturesSearchAborted(Path root) {}

void ImportFoldersService::imageProcessed(
    PBDev::ThumbnailsJobId jobId, std::tuple<Path, Path, Path> thumbnailPaths)
{
  auto root = mRootPaths.at(jobId);
  mListener->onImageProcessed(root, root,
                              PB::ImageResources{std::get<0>(thumbnailPaths),
                                                 std::get<1>(thumbnailPaths),
                                                 std::get<2>(thumbnailPaths)});
}

} // namespace PB