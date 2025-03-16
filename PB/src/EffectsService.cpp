#include <pb/services/EffectsService.h>

#include <pb/infra/FileSupport.h>

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

void EffectsService::configureProject(IdentifiableProject project)
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

void EffectsService::apply(PBDev::EffectId effectId, GenericImagePtr image,
                           float saturation, float brightness, float contrast,
                           bool inplace)
{
  mTaskCruncher->crunch([this, effectId, image, saturation, brightness,
                         contrast, inplace]() {
    applyInternal(effectId, image, saturation, brightness, contrast, inplace);
  });
}

void EffectsService::applyInternal(PBDev::EffectId effectId,
                                   GenericImagePtr image, float saturation,
                                   float brightness, float contrast,
                                   bool inplace)
{
  auto projectId = mProject->id;
  auto [largePath, mediumPath, smallPath] =
      mPlatformInfo->thumbnailPaths(projectId, image->hash());

  auto largeImage = std::make_shared<cv::Mat>(cv::imread(largePath.string()));
  auto mediumImage = std::make_shared<cv::Mat>(cv::imread(mediumPath.string()));
  auto smallImage = std::make_shared<cv::Mat>(cv::imread(smallPath.string()));

  Process::applySaturationInPlace(largeImage, saturation);
  Process::applyBrightnessInPlace(largeImage, brightness);
  Process::applyContrastInPlace(largeImage, contrast);

  Process::applySaturationInPlace(mediumImage, saturation);
  Process::applyBrightnessInPlace(mediumImage, brightness);
  Process::applyContrastInPlace(mediumImage, contrast);

  Process::applySaturationInPlace(smallImage, saturation);
  Process::applyBrightnessInPlace(smallImage, brightness);
  Process::applyContrastInPlace(smallImage, contrast);

  if (inplace) {
    infra::writeImageOnDisk(largeImage, largePath);
    infra::writeImageOnDisk(mediumImage, mediumPath);
    infra::writeImageOnDisk(smallImage, smallPath);

    mListener->onEffectsAppliedInplace(effectId);
  }
  else {
    auto newImage = mImageFactory->weakCopyImage(image);

    auto [newLargePath, newMediumPath, newSmallPath, hash] =
        mPlatformInfo->newThumbnailPaths(projectId, newImage->hash());

    infra::writeImageOnDisk(largeImage, newLargePath);
    infra::writeImageOnDisk(mediumImage, newMediumPath);
    infra::writeImageOnDisk(smallImage, newSmallPath);

    mListener->onEffectsApplied(effectId, newImage);
  }
}

} // namespace PB
