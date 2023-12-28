#include <pb/tasks/ThumbnailsProcessor.h>

#include <pb/Config.h>

namespace PB {
ResizeTask::ResizeTask(Path full, Path medium, Path small,
                       unsigned totalTaskCount, std::function<void()> onFinish,
                       int screenWidth, int screenHeight,
                       std::stop_token stopToken)
    : mFullSizePath(full), mSmallThumbnailOutputPath(small),
      mMediumThumbnailOutputPath(medium), mTotalTaskCount(totalTaskCount),
      mFinish(onFinish), mScreenWidth(screenWidth), mScreenHeight(screenHeight),
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
                                     mMediumThumbnailOutputPath,
                                     mSmallThumbnailOutputPath, resizeOption);
  }
  else {
    std::shared_ptr<cv::Mat> image =
        PB::Process::singleColorImage(3508, 2480, {255, 255, 255})();

    image = PB::Process::addText(
        {3508 / 2, 2480 / 2}, mFullSizePath.stem().string(), {0, 0, 0})(image);
    Process::imageWriteThumbnail(mScreenWidth, mScreenHeight, image,
                                 mMediumThumbnailOutputPath,
                                 mSmallThumbnailOutputPath);
  }
  mFinish();
}

ThumbnailsProcessor::ThumbnailsProcessor() {}

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
      mThumbnailWritten(inputPath, mediumPath, smallPath, i);
    };

    ResizeTask resizeTask(mediaMap.at(i), mediumPath, smallPath, taskCount,
                          task, mScreenWidth, mScreenHeight,
                          mStopSources.at(mStopSources.size() - 1).get_token());

    mParallelTaskConsumer.enqueue([resizeTask{resizeTask}]() { resizeTask(); });
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
  auto projectDetails = mProject->active();
  PBDev::basicAssert(index >= 0);
  PBDev::basicAssert(projectDetails.supportDirName().length() > 0);

  groupIdentifier = groupIdentifier + mProject->active().supportDirName();

  auto smallOutputPath = projectDetails.parentDirectory() /
                         projectDetails.supportDirName() /
                         (Context::SMALL_THUMBNAIL_NAME + groupIdentifier +
                          std::to_string(index) + Context::JPG_EXTENSION);

  auto mediumOutputPath = projectDetails.parentDirectory() /
                          projectDetails.supportDirName() /
                          (Context::MEDIUM_THUMBNAIL_NAME + groupIdentifier +
                           std::to_string(index) + Context::JPG_EXTENSION);

  return {smallOutputPath, mediumOutputPath};
}
} // namespace PB