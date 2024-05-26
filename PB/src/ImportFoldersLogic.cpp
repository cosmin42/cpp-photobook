#include <pb/ImportFoldersLogic.h>

#include <pb/util/Util.h>

namespace PB {
ImportFoldersLogic::ImportFoldersLogic(
    std::shared_ptr<PlatformInfo> platformInfo)
    : mThumbnailsProcessor(platformInfo)
{
}

void ImportFoldersLogic::configure(ImportFoldersLogicListener *listener)
{
  mListener = listener;
}

void ImportFoldersLogic::configure(PBDev::ThreadScheduler *scheduler)
{
  mScheduler = scheduler;
}

void ImportFoldersLogic::configure(std::pair<int, int> screenSize)
{
  mThumbnailsProcessor.setScreenSize(screenSize);
}
void ImportFoldersLogic::configure(std::shared_ptr<Project> project)
{
  mThumbnailsProcessor.provideProjectDetails(project);
  mProject = project;
}

void ImportFoldersLogic::setTaskCruncher(
    std::shared_ptr<TaskCruncher> taskCruncher)
{
  mTaskCruncher = taskCruncher;
}

std::optional<PBDev::Error> ImportFoldersLogic::addImportFolder(Path path)
{
  auto errorOrPath = PBDev::FileInfo::validInputRootPath(path);
  if (std::holds_alternative<PBDev::Error>(errorOrPath)) {
    return std::get<PBDev::Error>(errorOrPath);
  }

  for (auto &[key, value] : mPendingSearches) {
    if (PBDev::FileInfo::contains(key, path)) {
      return PBDev::Error() << PB::ErrorCode::FolderAlreadyImported;
    }
  }

  mPendingSearches.emplace(path, PicturesSearchConfig(path));
  mPendingSearches.at(path).setPicturesSearchConfigListener(this);
  mPendingSearches.at(path).assignUuid(
      PBDev::MapReducerTaskId(RuntimeUUID::newUUID()));
  mScheduler->post([this, path{path}]() { mListener->onMappingStarted(path); });
  mTaskCruncher->crunch("image-search-job", mPendingSearches.at(path));

  return std::nullopt;
}

void ImportFoldersLogic::stop(Path path) { mThumbnailsProcessor.abort(path); }

void ImportFoldersLogic::stopAll() { mTaskCruncher->abort(); }

void ImportFoldersLogic::clearJob(Path root)
{
  mImageProcessingProgress.erase(root);
}

void ImportFoldersLogic::onImageProcessed(Path key, Path root, Path full,
                                          Path medium, Path small,
                                          int progressCap)
{
  mScheduler->post([this, root, full, medium, small, progressCap, key{key}]() {
    if (mImageProcessingProgress.find(root) != mImageProcessingProgress.end()) {
      mImageProcessingProgress[root] = {
          mImageProcessingProgress.at(root).first + 1, progressCap};
    }
    else {
      mImageProcessingProgress[root] = {1, progressCap};
    }

    mListener->onImageProcessed(key, root, full, medium, small);
  });
}

void ImportFoldersLogic::processImages(std::string thumbnailsDirectoryName,
                                       RowProcessingData rowProcessingData,
                                       std::string       hash)
{
  mThumbnailsProcessor.generateThumbnails(
      thumbnailsDirectoryName, rowProcessingData.root, rowProcessingData.images,
      hash,
      [this, root{rowProcessingData.root},
       maxProgress{rowProcessingData.images.size()}](Path keyPath, Path full,
                                                     Path medium, Path small) {
        onImageProcessed(keyPath, root, full, medium, small, (int)maxProgress);
      });
}

void ImportFoldersLogic::onPicturesSearchFinished(
    Path root, std::vector<Path> searchResults)
{
  mScheduler->post([this, root{root}, searchResults{searchResults}]() {
    mPendingSearches.erase(root);
    if (searchResults.empty()) {
      mListener->onMappingAborted(root);
      mListener->onError(PBDev::Error() << ErrorCode::NoImages);
    }
    else {
      mListener->onMappingFinished(root, searchResults);
    }
  });
}

void ImportFoldersLogic::onPicturesSearchAborted(Path root)
{
  mScheduler->post([this, root{root}]() {
    mPendingSearches.erase(root);
    mListener->onMappingAborted(root);
  });
}

std::pair<int, int> ImportFoldersLogic::imageProcessingProgress() const
{
  int totalProgress = 0;
  int totalProgressCap = 0;

  for (auto [root, progress] : mImageProcessingProgress) {
    totalProgress += progress.first;
    totalProgressCap += progress.second;
  }

  return {totalProgress, totalProgressCap};
}

std::pair<int, int> ImportFoldersLogic::imageProcessingProgress(Path path) const
{
  return mImageProcessingProgress.at(path);
}

std::vector<Path> ImportFoldersLogic::runningImageProcessingJobs() const
{
  std::vector<Path> result;
  for (auto &[path, progress] : mImageProcessingProgress) {
    if (progress.first != progress.second) {
      result.push_back(path);
    }
  }
  return result;
}

std::vector<Path> ImportFoldersLogic::pendingMappingFolders() const
{
  std::vector<Path> keys;
  for (auto &[root, values] : mPendingSearches) {
    keys.push_back(root);
  }

  return keys;
}

void ImportFoldersLogic::markForDeletion(Path path)
{
  mRemovalMarks.insert(path);
}
void ImportFoldersLogic::removeMarkForDeletion(Path path)
{
  mRemovalMarks.erase(path);
  mThumbnailsProcessor.clearJob(path);
}

bool ImportFoldersLogic::marked(Path path) const
{
  return mRemovalMarks.contains(path);
}
} // namespace PB