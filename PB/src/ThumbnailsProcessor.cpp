#include <pb/tasks/ThumbnailsProcessor.h>

#include <pb/Config.h>
#include <pb/ImageMetadataLogic.h>
#include <pb/ImportImageTask.h>
#include <pb/RuntimeUUID.h>

namespace PB {

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
    std::function<void(Path, ImageResources)> onThumbnailWritten)
{

  mThumbnailWritten = onThumbnailWritten;

  for (auto i = 0; i < (int)mediaMap.size(); ++i) {
    auto &inputPath = mediaMap.at(i).resource;
    auto [smallPath, mediumPath] = assembleOutputPaths(
        mediaMap.at(i).position, groupIdentifier, thumbnailsDirectoryName);

    ImageResources imageResources = {inputPath, mediumPath, smallPath};

    auto task =
        [mThumbnailWritten{mThumbnailWritten}, keyPath{mediaMap.at(i).keyPath},
         imageResources{imageResources}](unsigned width, unsigned height) {
          ImageResources nextImageResources = imageResources;
          nextImageResources.width = width;
          nextImageResources.height = height;
          mThumbnailWritten(keyPath, nextImageResources);
        };

    ImportImageTask importImageTask(
        mediaMap.at(i).keyPath, mediumPath, smallPath, task, mScreenWidth,
        mScreenHeight, mStopSources[root].get_token());

    mParallelTaskConsumer.enqueue(
        [resizeTask{importImageTask}]() { resizeTask(); });
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
ThumbnailsProcessor::assembleOutputPaths(Path localStatePath, int index,
                                         std::string groupIdentifier,
                                         std::string projectName)
{
  PBDev::basicAssert(index >= 0);

  auto smallOutputPath = localStatePath / "th" / projectName /
                         (OneConfig::SMALL_THUMBNAIL_NAME + groupIdentifier +
                          std::to_string(index) + OneConfig::JPG_EXTENSION);

  auto mediumOutputPath = localStatePath / "th" / projectName /
                          (OneConfig::MEDIUM_THUMBNAIL_NAME + groupIdentifier +
                           std::to_string(index) + OneConfig::JPG_EXTENSION);

  return {smallOutputPath, mediumOutputPath};
}

std::pair<Path, Path>
ThumbnailsProcessor::assembleOutputPaths(int index, std::string groupIdentifier,
                                         std::string thumbnailsDirectoryName)
{
  return assembleOutputPaths(mPlatformInfo->localStatePath, index,
                             groupIdentifier, thumbnailsDirectoryName);
}
} // namespace PB