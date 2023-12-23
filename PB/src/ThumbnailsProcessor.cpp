#include <pb/tasks/ThumbnailsProcessor.h>

#include <pb/Config.h>

namespace PB {
ResizeTask::ResizeTask(Path fullSizePath, Path smallThumbnailOutputPath,
                       Path mediumThumbnailOutputPath, unsigned totalTaskCount,
                       std::function<void()> onFinish, int screenWidth,
                       int screenHeight, std::stop_token stopToken)
    : mFullSizePath(fullSizePath),
      mSmallThumbnailOutputPath(smallThumbnailOutputPath),
      mMediumThumbnailOutputPath(mediumThumbnailOutputPath),
      mTotalTaskCount(totalTaskCount), mFinish(onFinish),
      mScreenWidth(screenWidth), mScreenHeight(screenHeight),
      mStopToken(stopToken)
{
}

void ResizeTask::operator()() const
{
  if (mStopToken.stop_requested()) {
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
                                     mSmallThumbnailOutputPath,
                                     mMediumThumbnailOutputPath, resizeOption);
  }
  else {
    std::shared_ptr<cv::Mat> image =
        PB::Process::singleColorImage(3508, 2480, {255, 255, 255})();

    image = PB::Process::addText(
        {3508 / 2, 2480 / 2}, mFullSizePath.stem().string(), {0, 0, 0})(image);
    Process::imageWriteThumbnail(mScreenWidth, mScreenHeight, image,
                                 mSmallThumbnailOutputPath,
                                 mMediumThumbnailOutputPath);
  }
  mFinish();
}

ThumbnailsProcessor::ThumbnailsProcessor()
    : mResizePool(Context::THUMBNAIL_THREADPOOL_THREAD_COUNT)
{
}

ThumbnailsProcessor::~ThumbnailsProcessor() { abort(); }

void ThumbnailsProcessor::setScreenSize(std::pair<int, int> screenSize)
{
  mScreenWidth = screenSize.first;
  mScreenHeight = screenSize.second;
}

void ThumbnailsProcessor::provideProjectDetails(
    ProjectSnapshot const &projectDetails)
{
  mProjectDetails = projectDetails;
}

void ThumbnailsProcessor::generateThumbnails(
    std::vector<Path> mediaMap, std::string groupIdentifier,
    std::function<void(Path, Path, Path, int)> onThumbnailWritten)
{
  mStopSources.push_back(std::stop_source());

  mThumbnailWritten = onThumbnailWritten;
  unsigned taskCount = (unsigned)mediaMap.size();

  for (auto i = 0; i < (int)mediaMap.size(); ++i) {
    auto &inputPath = mediaMap.at(i);
    auto [smallPath, mediumPath] = assembleOutputPaths(i, groupIdentifier);

    auto task = [mThumbnailWritten{mThumbnailWritten}, inputPath{inputPath},
                 smallPath{smallPath}, mediumPath{mediumPath}, i{i},
                 taskCount{taskCount}]() {
      mThumbnailWritten(inputPath, smallPath, mediumPath, i);
    };

    ResizeTask resizeTask(mediaMap.at(i), smallPath, mediumPath, taskCount,
                          task, mScreenWidth, mScreenHeight,
                          mStopSources.at(mStopSources.size() - 1).get_token());
    std::future<void> token = mResizePool.enqueue(resizeTask);

    mFutures.push_back(std::move(token));
  }
}

void ThumbnailsProcessor::abort()
{
  for (auto &source : mStopSources) {
    source.request_stop();
  }

  for (auto &f : mFutures) {
    f.wait();
  }
}

std::pair<Path, Path>
ThumbnailsProcessor::assembleOutputPaths(int index, std::string groupIdentifier)
{
  PBDev::basicAssert(index >= 0);
  PBDev::basicAssert(mProjectDetails.supportDirName().length() > 0);

  auto smallOutputPath = mProjectDetails.parentDirectory() /
                         mProjectDetails.supportDirName() /
                         (Context::SMALL_THUMBNAIL_NAME + groupIdentifier +
                          std::to_string(index) + Context::JPG_EXTENSION);

  auto mediumOutputPath = mProjectDetails.parentDirectory() /
                          mProjectDetails.supportDirName() /
                          (Context::MEDIUM_THUMBNAIL_NAME + groupIdentifier +
                           std::to_string(index) + Context::JPG_EXTENSION);

  return {smallOutputPath, mediumOutputPath};
}
} // namespace PB