#include <pb/ImportFoldersLogic.h>

#include <pb/util/Util.h>

namespace PB {
int ImportFoldersLogic::thumbnailsDir = 0;

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

void ImportFoldersLogic::stop(Path path) {}

void ImportFoldersLogic::stopAll() {}

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
  mThumbnailsProcessor.generateThumbnails(
      newFolders, std::to_string(thumbnailsDir),
      [this, root{root}, maxProgress{newFolders.size()}](Path full, Path medium,
                                                         Path small) {
        onImageProcessed(root, full, medium, small, (int)maxProgress);
      });
  thumbnailsDir++;
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
} // namespace PB