#include <pb/components/ImportImageTask.h>

#include <pb/Enums.h>

#include <pb/Config.h>
#include <pb/components/ImageMetadataInspector.h>
#include <pb/image/ImageOperations.h>

namespace PB {
ImportImageTask::ImportImageTask(
    Path full, Path medium, Path small,
    std::function<void(unsigned, unsigned)> onFinish, int screenWidth,
    int screenHeight, std::stop_token stopToken)
    : mFullSizePath(full), mSmallThumbnailOutputPath(small),
      mMediumThumbnailOutputPath(medium), mFinish(onFinish),
      mScreenWidth(screenWidth), mScreenHeight(screenHeight),
      mStopToken(stopToken)
{
}

void ImportImageTask::operator()() const
{
  if (mStopToken.stop_requested()) {
    mFinish(0, 0);
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

  ImageMetadataInspector imageMetadatainspector(mFullSizePath);

  imageMetadatainspector.inspect();

  auto width = imageMetadatainspector.width();
  auto height = imageMetadatainspector.height();

  if constexpr (OneConfig::SIMULATE_SLOW_THUMBNAILS_PROCESSOR) {
    std::this_thread::sleep_for(std::chrono::milliseconds(3000));
  }
  mFinish(width, height);
}
} // namespace PB
