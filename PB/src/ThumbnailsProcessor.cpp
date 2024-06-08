#include <pb/tasks/ThumbnailsProcessor.h>

#include <pb/Config.h>
#include <pb/RuntimeUUID.h>

namespace PB {
ResizeTask::ResizeTask(Path full, Path medium, Path small,
                       std::function<void()> onFinish, int screenWidth,
                       int screenHeight, std::stop_token stopToken)
    : mFullSizePath(full), mSmallThumbnailOutputPath(small),
      mMediumThumbnailOutputPath(medium), mFinish(onFinish),
      mScreenWidth(screenWidth), mScreenHeight(screenHeight),
      mStopToken(stopToken)
{
}

void ResizeTask::operator()() const
{
  if (mStopToken.stop_requested()) {
    mFinish();
    return;
  }
  auto resizeOption = ThumbnailType::None;
  if (!std::filesystem::exists(mSmallThumbnailOutputPath)) {
    resizeOption = ThumbnailType::Small;
  }
  if (!std::filesystem::exists(mMediumThumbnailOutputPath)) {
    resizeOption = (resizeOption | ThumbnailType::Medium);
  }
  if (Process::validExtension(mFullSizePath)) {
    Process::readImageWriteThumbnail(mScreenWidth, mScreenHeight, mFullSizePath,
                                     mMediumThumbnailOutputPath,
                                     mSmallThumbnailOutputPath, resizeOption);
  }
  else {
    PBDev::basicAssert(false);
  }
  if constexpr (OneConfig::SIMULATE_SLOW_THUMBNAILS_PROCESSOR) {
    std::this_thread::sleep_for(std::chrono::milliseconds(3000));
  }
  mFinish();
}

ThumbnailsProcessor::ThumbnailsProcessor(
    std::shared_ptr<PlatformInfo> platformInfo)
    : mParallelTaskConsumer(OneConfig::THUMBNAIL_THREADPOOL_THREAD_COUNT),
      mPlatformInfo(platformInfo)
{
}

ThumbnailsProcessor::~ThumbnailsProcessor() { abort(); }

void ThumbnailsProcessor::setScreenSize(std::pair<int, int> screenSize)
{
  mScreenWidth = screenSize.first;
  mScreenHeight = screenSize.second;
}

void ThumbnailsProcessor::provideProjectDetails(
    std::shared_ptr<Project> project)
{
  mProject = project;
}

void ThumbnailsProcessor::generateThumbnails(
    std::string thumbnailsDirectoryName, Path root,
    std::vector<ProcessingData> mediaMap, std::string groupIdentifier,
    std::function<void(Path, Path, Path, Path)> onThumbnailWritten)
{

  mThumbnailWritten = onThumbnailWritten;

  for (auto i = 0; i < (int)mediaMap.size(); ++i) {
    auto &inputPath = mediaMap.at(i).resource;
    auto [smallPath, mediumPath] = assembleOutputPaths(
        mediaMap.at(i).position, groupIdentifier, thumbnailsDirectoryName);

    auto task = [mThumbnailWritten{mThumbnailWritten},
                 keyPath{mediaMap.at(i).keyPath}, inputPath{inputPath},
                 smallPath{smallPath}, mediumPath{mediumPath}]() {
      mThumbnailWritten(keyPath, inputPath, mediumPath, smallPath);
    };

    ResizeTask resizeTask(mediaMap.at(i).keyPath, mediumPath, smallPath, task,
                          mScreenWidth, mScreenHeight,
                          mStopSources[root].get_token());

    mParallelTaskConsumer.enqueue([resizeTask{resizeTask}]() { resizeTask(); });
  }
}

void ThumbnailsProcessor::abort()
{
  for (auto &[path, source] : mStopSources) {
    PBDev::Unused(path);
    source.request_stop();
  }

  mParallelTaskConsumer.wait();

  clearJobs();
}

void ThumbnailsProcessor::abort(Path path)
{
  if (mStopSources.find(path) != mStopSources.end()) {
    mStopSources.at(path).request_stop();
  }
}

void ThumbnailsProcessor::clearJob(Path path) { mStopSources.erase(path); }

void ThumbnailsProcessor::clearJobs() { mStopSources.clear(); }

std::pair<Path, Path>
ThumbnailsProcessor::assembleOutputPaths(int index, std::string groupIdentifier,
                                         std::string thumbnailsDirectoryName)
{
  PBDev::basicAssert(index >= 0);

  auto smallOutputPath = mPlatformInfo->localStatePath / "th" /
                         thumbnailsDirectoryName /
                         (OneConfig::SMALL_THUMBNAIL_NAME + groupIdentifier +
                          std::to_string(index) + OneConfig::JPG_EXTENSION);

  auto mediumOutputPath = mPlatformInfo->localStatePath / "th" /
                          thumbnailsDirectoryName /
                          (OneConfig::MEDIUM_THUMBNAIL_NAME + groupIdentifier +
                           std::to_string(index) + OneConfig::JPG_EXTENSION);

  return {smallOutputPath, mediumOutputPath};
}
} // namespace PB