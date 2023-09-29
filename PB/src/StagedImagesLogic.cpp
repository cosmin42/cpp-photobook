#include <pb/StagedImagesLogic.h>

namespace PB {
ResizeTask::ResizeTask(Path fullSizePath, Path outputPath,
                       unsigned totalTaskCount, std::function<void()> onFinish)
    : mFullSizePath(fullSizePath), mOutputPath(outputPath),
      mTotalTaskCount(totalTaskCount), mFinish(onFinish)
{
}

void ResizeTask::operator()() const
{
  if (MediaMap::validImagePath(mFullSizePath)) {
    Process::readImageWriteThumbnail(mFullSizePath, mOutputPath);
  }
  else {
    std::shared_ptr<cv::Mat> image =
        PB::Process::singleColorImage(1280, 640, {255, 0, 0})();

    image = PB::Process::addText({1280 / 2, 640 / 2},
                                 mFullSizePath.parent_path().string(),
                                 {0, 255, 0})(image);
    Process::imageWriteThumbnail(image, mOutputPath);
  }
  mFinish();
}

StagedImagesLogic::~StagedImagesLogic()
{
  for (auto &f : mFutures) {
    f.wait();
  }
}

void StagedImagesLogic::provideProjectDetails(
    ProjectDetails const &projectDetails)
{
  mProjectDetails = projectDetails;
}

void StagedImagesLogic::generateThumbnails(
    MediaMap &mediaMap,
    std::function<void(Path, Path)> onThumbnailWritten)
{
  mThumbnailWritten = onThumbnailWritten;
  unsigned taskCount = mediaMap.size();

  for (auto i = 0; i < (int)mediaMap.size(); ++i) {
    auto inputPath = mediaMap.map().at(i);
    auto outputPath = assembleOutputPath(i);

    ResizeTask        resizeTask(mediaMap.map().at(i), outputPath, taskCount,
                                 [mThumbnailWritten{mThumbnailWritten},
                           inputPath{inputPath}, outputPath{outputPath}]() {
                            mThumbnailWritten(inputPath, outputPath);
                          });
    std::future<void> token = mResizePool.enqueue(resizeTask);

    mFutures.push_back(std::move(token));
  }
}

Path StagedImagesLogic::assembleOutputPath(int index)
{
  assert(index >= 0);
  assert(mProjectDetails.dirName.length() > 0);

  auto path = mProjectDetails.parentDirectory / mProjectDetails.dirName /
              (sPrefix + std::to_string(index) + Context::jpgExt);

  return path;
}
} // namespace PB