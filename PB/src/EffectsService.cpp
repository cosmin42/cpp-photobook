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

void EffectsService::applySaturation(PBDev::EffectId effectId,
                                     GenericImagePtr image, float saturation,
                                     bool inplace)
{
  mTaskCruncher->crunch([this, effectId, image, saturation, inplace]() {
    applySaturationInternal(effectId, image, saturation, inplace);
  });
}

void EffectsService::applyBrightness(PBDev::EffectId effectId,
                                     GenericImagePtr image, float brightness,
                                     bool inplace)
{
  mTaskCruncher->crunch([this, effectId, image, brightness, inplace]() {
    applyBrightnessInternal(effectId, image, brightness, inplace);
  });
}

void EffectsService::applyContrast(PBDev::EffectId effectId,
                                   GenericImagePtr image, float contrast,
                                   bool inplace)
{
  mTaskCruncher->crunch([this, effectId, image, contrast, inplace]() {
    applyContrastInternal(effectId, image, contrast, inplace);
  });
}

void EffectsService::applySaturationInternal(PBDev::EffectId effectId,
                                             GenericImagePtr image,
                                             float saturation, bool inplace)
{
  auto projectId = mProject->first;
  auto [largePath, mediumPath, smallPath] =
      mPlatformInfo->thumbnailPaths(projectId, image->hash());

  auto largeImage = std::make_shared<cv::Mat>(cv::imread(largePath.string()));
  auto mediumImage = std::make_shared<cv::Mat>(cv::imread(mediumPath.string()));
  auto smallImage = std::make_shared<cv::Mat>(cv::imread(smallPath.string()));

  if (inplace) {
    Process::applySaturationInPlace(largeImage, saturation);
    Process::applySaturationInPlace(mediumImage, saturation);
    Process::applySaturationInPlace(smallImage, saturation);
    mListener->onEffectApplied(effectId, image);
  }
  else {
    largeImage = Process::applySaturation(largeImage, saturation);
    mediumImage = Process::applySaturation(mediumImage, saturation);
    smallImage = Process::applySaturation(smallImage, saturation);

    auto newImage = mImageFactory->weakCopyImage(image);

    Process::writeImageOnDisk(largeImage, newImage->full());
    Process::writeImageOnDisk(mediumImage, newImage->medium());
    Process::writeImageOnDisk(smallImage, newImage->smaLL());

    mListener->onEffectApplied(effectId, newImage);
  }
}

void EffectsService::applyBrightnessInternal(PBDev::EffectId effectId,
                                             GenericImagePtr image,
                                             float brightness, bool inplace)
{
  auto projectId = mProject->first;
  auto [largePath, mediumPath, smallPath] =
      mPlatformInfo->thumbnailPaths(projectId, image->hash());

  auto largeImage = std::make_shared<cv::Mat>(cv::imread(largePath.string()));
  auto mediumImage = std::make_shared<cv::Mat>(cv::imread(mediumPath.string()));
  auto smallImage = std::make_shared<cv::Mat>(cv::imread(smallPath.string()));

  if (inplace) {
    Process::applyBrightnessInPlace(largeImage, brightness);
    Process::applyBrightnessInPlace(mediumImage, brightness);
    Process::applyBrightnessInPlace(smallImage, brightness);
    mListener->onEffectApplied(effectId, image);
  }
  else {
    largeImage = Process::applyBrightness(largeImage, brightness);
    mediumImage = Process::applyBrightness(mediumImage, brightness);
    smallImage = Process::applyBrightness(smallImage, brightness);

    auto newImage = mImageFactory->weakCopyImage(image);

    Process::writeImageOnDisk(largeImage, newImage->full());
    Process::writeImageOnDisk(mediumImage, newImage->medium());
    Process::writeImageOnDisk(smallImage, newImage->smaLL());

    mListener->onEffectApplied(effectId, newImage);
  }
}

void EffectsService::applyContrastInternal(PBDev::EffectId effectId,
                                           GenericImagePtr image,
                                           float contrast, bool inplace)
{
  auto projectId = mProject->first;
  auto [largePath, mediumPath, smallPath] =
      mPlatformInfo->thumbnailPaths(projectId, image->hash());

  auto largeImage = std::make_shared<cv::Mat>(cv::imread(largePath.string()));
  auto mediumImage = std::make_shared<cv::Mat>(cv::imread(mediumPath.string()));
  auto smallImage = std::make_shared<cv::Mat>(cv::imread(smallPath.string()));

  if (inplace) {
    Process::applyContrastInPlace(largeImage, contrast);
    Process::applyContrastInPlace(mediumImage, contrast);
    Process::applyContrastInPlace(smallImage, contrast);

    mListener->onEffectApplied(effectId, image);
  }
  else {
    largeImage = Process::applyContrast(largeImage, contrast);
    mediumImage = Process::applyContrast(mediumImage, contrast);
    smallImage = Process::applyContrast(smallImage, contrast);

    auto newImage = mImageFactory->weakCopyImage(image);

    Process::writeImageOnDisk(largeImage, newImage->full());
    Process::writeImageOnDisk(mediumImage, newImage->medium());
    Process::writeImageOnDisk(smallImage, newImage->smaLL());

    mListener->onEffectApplied(effectId, newImage);
  }
}

} // namespace PB
