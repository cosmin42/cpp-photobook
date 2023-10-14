#include <pb/ThumbnailsProcessor.h>

#include <pb/Config.h>

namespace PB {
ResizeTask::ResizeTask(Path fullSizePath, Path smallThumbnailOutputPath,
                       Path mediumThumbnailOutputPath, unsigned totalTaskCount,
                       std::function<void()> onFinish, int screenWidth,
                       int screenHeight)
    : mFullSizePath(fullSizePath),
      mSmallThumbnailOutputPath(smallThumbnailOutputPath),
      mMediumThumbnailOutputPath(mediumThumbnailOutputPath),
      mTotalTaskCount(totalTaskCount), mFinish(onFinish),
      mScreenWidth(screenWidth), mScreenHeight(screenHeight)
{
}

void ResizeTask::operator()() const
{
  if (MediaMap::validImagePath(mFullSizePath)) {
    Process::readImageWriteThumbnail(mScreenWidth, mScreenHeight, mFullSizePath,
                                     mSmallThumbnailOutputPath,
                                     mMediumThumbnailOutputPath);
  }
  else {
    std::shared_ptr<cv::Mat> image =
        PB::Process::singleColorImage(3508, 2480, {255, 255, 255})();

    image = PB::Process::addText({3508 / 2, 2480 / 2},
                                 mFullSizePath.filename().string(),
                                 {0, 0, 0})(image);
    Process::imageWriteThumbnail(mScreenWidth, mScreenHeight, image,
                                 mSmallThumbnailOutputPath,
                                 mMediumThumbnailOutputPath);
  }
  mFinish();
}

ThumbnailsProcessor::ThumbnailsProcessor(std::pair<int, int> size)
    : mResizePool(Context::THUMBNAIL_THREADPOOL_THREAD_COUNT),
      mScreenWidth(size.first), mScreenHeight(size.second)
{
}

ThumbnailsProcessor::~ThumbnailsProcessor()
{
  for (auto &f : mFutures) {
    f.wait();
  }
}

void ThumbnailsProcessor::provideProjectDetails(
    ProjectDetails const &projectDetails)
{
  mProjectDetails = projectDetails;
}

void ThumbnailsProcessor::generateThumbnails(
    std::vector<Path> mediaMap, std::string groupIdentifier,
    std::function<void(Path, Path, Path, int)> onThumbnailWritten)
{
  mThumbnailWritten = onThumbnailWritten;
  unsigned taskCount = (unsigned)mediaMap.size();

  for (auto i = 0; i < (int)mediaMap.size(); ++i) {
    auto inputPath = mediaMap.at(i);
    auto [smallPath, mediumPath] = assembleOutputPaths(i, groupIdentifier);

    auto task = [mThumbnailWritten{mThumbnailWritten}, inputPath{inputPath},
                 smallPath{smallPath}, mediumPath{mediumPath}, i{i},
                 taskCount{taskCount}]() {
      mThumbnailWritten(inputPath, smallPath, mediumPath, i);
    };

    ResizeTask resizeTask(mediaMap.at(i), smallPath, mediumPath, taskCount,
                          task, mScreenWidth, mScreenHeight);
    std::future<void> token = mResizePool.enqueue(resizeTask);

    mFutures.push_back(std::move(token));
  }
}

std::pair<Path, Path>
ThumbnailsProcessor::assembleOutputPaths(int index, std::string groupIdentifier)
{
  PB::basicAssert(index >= 0);
  PB::basicAssert(mProjectDetails.supportDirName.length() > 0);

  auto smallOutputPath = mProjectDetails.parentDirectory /
                         mProjectDetails.supportDirName /
                         (Context::SMALL_THUMBNAIL_NAME + groupIdentifier +
                          std::to_string(index) + Context::JPG_EXTENSION);

  auto mediumOutputPath = mProjectDetails.parentDirectory /
                          mProjectDetails.supportDirName /
                          (Context::MEDIUM_THUMBNAIL_NAME + groupIdentifier +
                           std::to_string(index) + Context::JPG_EXTENSION);

  return {smallOutputPath, mediumOutputPath};
}
} // namespace PB