#include <pb/ImportFoldersLogic.h>

#include <pb/util/Util.h>

namespace PB {
void ImportFoldersLogic::configure(ImportFoldersLogicListener *listener)
{
  mListener = listener;
}

void ImportFoldersLogic::configure(ThreadScheduler *scheduler)
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

std::optional<PBDev::Error> ImportFoldersLogic::addImportFolder(Path path)
{
  auto errorOrPath = PBDev::FileInfo::validInputRootPath(path);
  if (std::holds_alternative<PBDev::Error>(errorOrPath)) {
    return std::get<PBDev::Error>(errorOrPath);
  }

  for (auto &[key, value] : mMappingJobs) {
    if (PBDev::FileInfo::contains(key, path)) {
      return PBDev::Error() << PB::ErrorCode::FolderAlreadyImported;
    }
  }

  MediaMapper mapper(path);

  mMappingJobs[path] = PBDev::SequentialTaskConsumer<MediaMapper>();

  mMappingJobs.at(path).configure(mapper);

  mMappingJobs.at(path).configure(
      (SequentialTaskConsumerListener<MediaMapper> *)this);

  return std::nullopt;
}

void ImportFoldersLogic::start(Path path)
{
  PB::printDebug("Starting %s\n", path.string().c_str());
  mMappingJobs.at(path).start();
}

void ImportFoldersLogic::stop(Path path) { mThumbnailsProcessor.abort(path); }

void ImportFoldersLogic::stopAll()
{
  for (auto &mappingJob : mMappingJobs) {
    mThumbnailsProcessor.abort(mappingJob.first);
  }
}

void ImportFoldersLogic::clearJob(Path root)
{
  mMappingJobs.erase(root);
  mImageProcessingProgress.erase(root);
}

void ImportFoldersLogic::onImageProcessed(Path root, Path full, Path medium,
                                          Path small, int progressCap)
{
  mScheduler->post([this, root, full, medium, small, progressCap]() {
    if (mImageProcessingProgress.find(root) != mImageProcessingProgress.end()) {
      mImageProcessingProgress[root] = {
          mImageProcessingProgress.at(root).first + 1, progressCap};
    }
    else {
      mImageProcessingProgress[root] = {1, progressCap};
    }

    mListener->onImageProcessed(root, full, medium, small);
  });
}

void ImportFoldersLogic::processImages(Path root, std::vector<Path> newFolders)
{
  auto pathHash = mProject->active().pathCache;
  auto hash = pathHash.hashCreateIfMissing(root);

  mThumbnailsProcessor.generateThumbnails(
      root, newFolders, hash,
      [this, root{root}, maxProgress{newFolders.size()}](Path full, Path medium,
                                                         Path small) {
        onImageProcessed(root, full, medium, small, (int)maxProgress);
      });
}

void ImportFoldersLogic::started(MediaMapper const &mediaMap)
{
  mScheduler->post([this, mediaMap{mediaMap}]() {
    mListener->onMappingStarted(mediaMap.root());
  });
}

void ImportFoldersLogic::finished(MediaMapper const &mediaMap)
{
  mScheduler->post([this, mediaMap{mediaMap}]() {
    mListener->onMappingFinished(mediaMap.root(),
                                 mediaMap.importedDirectories());
  });
}

void ImportFoldersLogic::aborted(MediaMapper const &mediaMap)
{
  mScheduler->post([this, mediaMap{mediaMap}]() {
    mListener->onMappingAborted(mediaMap.root());
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

std::vector<Path> ImportFoldersLogic::pendingMappingFolders() const
{
  std::vector<Path> keys;
  for (auto &[key, value] : mMappingJobs) {
    keys.push_back(key);
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