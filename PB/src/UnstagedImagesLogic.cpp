#include <pb/UnstagedImagesLogic.h>

namespace PB {
ResizeTask::ResizeTask(Path fullSizePath, Path smallThumbnailOutputPath,
                       Path mediumThumbnailOutputPath, unsigned totalTaskCount,
                       std::function<void()> onFinish)
    : mFullSizePath(fullSizePath),
      mSmallThumbnailOutputPath(smallThumbnailOutputPath),
      mMediumThumbnailOutputPath(mediumThumbnailOutputPath),
      mTotalTaskCount(totalTaskCount), mFinish(onFinish)
{
}

void ResizeTask::operator()() const
{
  if (MediaMap::validImagePath(mFullSizePath)) {
    Process::readImageWriteThumbnail(mFullSizePath, mSmallThumbnailOutputPath,
                                     mMediumThumbnailOutputPath);
  }
  else {
    std::shared_ptr<cv::Mat> image =
        PB::Process::singleColorImage(1280, 640, {255, 0, 0})();

    image = PB::Process::addText({1280 / 2, 640 / 2},
                                 mFullSizePath.parent_path().string(),
                                 {0, 255, 0})(image);
    Process::imageWriteThumbnail(image, mSmallThumbnailOutputPath, mMediumThumbnailOutputPath);
  }
  mFinish();
}

UnstagedImagesLogic::UnstagedImagesLogic() : mResizePool(sNumberOfThreads) {}

UnstagedImagesLogic::~UnstagedImagesLogic()
{
  for (auto &f : mFutures) {
    f.wait();
  }
}

void UnstagedImagesLogic::provideProjectDetails(
    ProjectDetails const &projectDetails)
{
  mProjectDetails = projectDetails;
}

void UnstagedImagesLogic::generateThumbnails(
    MediaMap &mediaMap, std::function<void(Path, Path, Path)> onThumbnailWritten)
{
  mThumbnailWritten = onThumbnailWritten;
  unsigned taskCount = mediaMap.size();

  for (auto i = 0; i < (int)mediaMap.size(); ++i) {
    auto inputPath = mediaMap.map().at(i);
    auto [smallPath, mediumPath] = assembleOutputPaths(i);

    ResizeTask        resizeTask(mediaMap.map().at(i), smallPath, mediumPath, taskCount,
                                 [mThumbnailWritten{mThumbnailWritten},
                           inputPath{inputPath}, smallPath{smallPath},
                           mediumPath{mediumPath}]() {
                            mThumbnailWritten(inputPath, smallPath, mediumPath);
                          });
    std::future<void> token = mResizePool.enqueue(resizeTask);

    mFutures.push_back(std::move(token));
  }
}

std::pair<Path, Path> UnstagedImagesLogic::assembleOutputPaths(int index)
{
  assert(index >= 0);
  assert(mProjectDetails.dirName.length() > 0);

  auto smallOutputPath =
      mProjectDetails.parentDirectory / mProjectDetails.dirName /
      (sSmallThumbnailPrefix + std::to_string(index) + Context::jpgExt);

  auto mediumOutputPath =
      mProjectDetails.parentDirectory / mProjectDetails.dirName /
      (sMediumThumbnailPrefix + std::to_string(index) + Context::jpgExt);

  return {smallOutputPath, mediumOutputPath};
}
} // namespace PB