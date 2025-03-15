#include <pb/services/EffectsService.h>

namespace PB {

void EffectsService::configureEffectsServiceListener(
    EffectsServiceListener *listener)
{
  mListener = listener;
}

void EffectsService::configurePlatformInfo(
    std::shared_ptr<PlatformInfo> platformInfo)
{
  mPlatformInfo = platformInfo;
}

void EffectsService::configureProject(
    std::shared_ptr<IdentifyableProject> project)
{
  mProject = project;
}

void EffectsService::configureImageFactory(
    std::shared_ptr<ImageFactory> imageFactory)
{
  mImageFactory = imageFactory;
}

void EffectsService::configureTaskCruncher(
    std::shared_ptr<TaskCruncher> taskCruncher)
{
  mTaskCruncher = taskCruncher;
}

void EffectsService::apply(PBDev::EffectId effectId, GenericImagePtr image, float saturation,
  float brightness, float contrast, bool inplace)
{
  mTaskCruncher->crunch([this, effectId, image, saturation, brightness, contrast, inplace]() {
    applyInternal(effectId, image, saturation, brightness, contrast, inplace);
  });
}

void EffectsService::applyInternal(PBDev::EffectId effectId, GenericImagePtr image, float saturation,
                          float brightness, float contrast, bool inplace)
{
  auto projectId = mProject->first;
  auto [largePath, mediumPath, smallPath] =
      mPlatformInfo->thumbnailPaths(projectId, image->hash());

  auto largeImage = std::make_shared<cv::Mat>(cv::imread(largePath.string()));
  auto mediumImage = std::make_shared<cv::Mat>(cv::imread(mediumPath.string()));
  auto smallImage = std::make_shared<cv::Mat>(cv::imread(smallPath.string()));

  if (inplace) {
    Process::applySaturationInPlace(largeImage, contrast);
    Process::applyBrightnessInPlace(largeImage, brightness);
    Process::applyContrastInPlace(largeImage, contrast);

    Process::applySaturationInPlace(mediumImage, contrast);
    Process::applyBrightnessInPlace(mediumImage, brightness);
    Process::applyContrastInPlace(mediumImage, contrast);

    mListener->onEffectsAppliedInplace(effectId);
  }
  else {
    
    largeImage = Process::applySaturation(largeImage, contrast);
    largeImage = Process::applyBrightness(largeImage, brightness);
    largeImage = Process::applyContrast(largeImage, contrast);

    mediumImage = Process::applySaturation(mediumImage, contrast);
    mediumImage = Process::applyBrightness(mediumImage, brightness);
    mediumImage = Process::applyContrast(mediumImage, contrast);

    smallImage = Process::applySaturation(smallImage, contrast);
    smallImage = Process::applyBrightness(smallImage, brightness);
    smallImage = Process::applyContrast(smallImage, contrast);

    auto newImage = mImageFactory->weakCopyImage(image);

    auto [newLargePath, newMediumPath, newSmallPath, hash] =
      mPlatformInfo->newThumbnailPaths(projectId, newImage->hash());

    Process::writeImageOnDisk(largeImage, newLargePath);
    Process::writeImageOnDisk(mediumImage, newMediumPath);
    Process::writeImageOnDisk(smallImage, newSmallPath);

    mListener->onEffectsApplied(effectId, newImage);
  }
}

} // namespace PB
