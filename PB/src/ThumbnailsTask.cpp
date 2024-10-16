#include <pb/components/ThumbnailsTask.h>

#include <pb/components/ImageMetadataInspector.h>
#include <pb/image/ImageReader.h>

namespace PB {
ThumbnailsTask::ThumbnailsTask(Path originalPath) : mOriginalPath(originalPath)
{
}

void ThumbnailsTask::configurePlatformInfo(
    std::shared_ptr<PlatformInfo> platformInfo)
{
  mPlatformInfo = platformInfo;
}

void ThumbnailsTask::configureProjectManagementService(
    std::shared_ptr<ProjectManagementService> projectManagementService)
{
  mProjectManagementService = projectManagementService;
}

std::string ThumbnailsTask::createThumbnails()
{
  auto maybleProject = mProjectManagementService->maybeLoadedProjectInfo();
  PBDev::basicAssert(maybleProject != nullptr);

  auto projectId = maybleProject->first;

  auto [large, medium, small, hash] =
      mPlatformInfo->newThumbnailPaths(projectId);

  ImageMetadataInspector imageMetadataInspector(mOriginalPath);

  imageMetadataInspector.inspect();

  auto width = imageMetadataInspector.width();
  auto height = imageMetadataInspector.height();

  auto [largeSize, mediumSize, smallSize] =
      thumbnailSizes(cv::Size{(int)width, (int)height},
                     {(int)maybleProject->second.paperSettings.width,
                      (int)maybleProject->second.paperSettings.height});

  auto originalImage = ImageReader().loadImage(mOriginalPath);

  auto smallImage = PB::Process::clone(originalImage);
  auto mediumImage = PB::Process::clone(originalImage);
  auto largeImage = PB::Process::clone(originalImage);

  cv::resize(*smallImage, *smallImage, smallSize);
  cv::resize(*mediumImage, *mediumImage, mediumSize);
  cv::resize(*largeImage, *largeImage, largeSize);

  PB::Process::writeImageOnDisk(smallImage, small);
  PB::Process::writeImageOnDisk(mediumImage, medium);
  PB::Process::writeImageOnDisk(largeImage, large);

  return hash;
}

std::tuple<cv::Size, cv::Size, cv::Size>
ThumbnailsTask::thumbnailSizes(cv::Size originalSize, cv::Size paperSize)
{
  auto smallSize = PB::Geometry::scaleToFitBoundingBox(
      originalSize, cv::Size{OneConfig::SMALL_THUMBNAIL_WIDTH,
                             OneConfig::SMALL_THUMBNAIL_HEIGHT});

  auto mediumSize = PB::Geometry::scaleToFitBoundingBox(
      originalSize, cv::Size{OneConfig::MEDIUM_THUMBNAIL_WIDTH,
                             OneConfig::MEDIUM_THUMBNAIL_HEIGHT});

  if (mediumSize.width > originalSize.width) {
    mediumSize = originalSize;
  }

  auto largeSize = PB::Geometry::scaleToFillBoundingBox(
      originalSize, cv::Size{paperSize.width, paperSize.height});

  if (largeSize.width > originalSize.width) {
    largeSize = originalSize;
  }

  return {largeSize, mediumSize, smallSize};
}

} // namespace PB
