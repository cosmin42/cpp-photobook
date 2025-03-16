#include <pb/components/ThumbnailsTask.h>

#include <pb/components/ImageMetadataInspector.h>
#include <pb/infra/FileSupport.h>

namespace PB {

// TODO: reconcile with the other createThumbnails
std::string
ThumbnailsTask::createThumbnails(std::shared_ptr<cv::Mat>      originalImage,
                                 std::shared_ptr<PlatformInfo> platformInfo,
                                 std::shared_ptr<IdentifyableProject> project,
                                 std::string targetHash)
{

  auto projectId = project->first;

  if (targetHash.empty()) {
    targetHash = "wait";
  }

  auto [large, medium, small, hash] =
      platformInfo->newThumbnailPaths(projectId, targetHash);

  auto width = originalImage->cols;
  auto height = originalImage->rows;

  auto [largeSize, mediumSize, smallSize] = Geometry::compute3SizesGeometry(
      cv::Size{(int)width, (int)height},
      {(int)project->second.paperSettings.width,
       (int)project->second.paperSettings.height});

  auto smallImage = PB::Process::clone(originalImage);
  auto mediumImage = PB::Process::clone(originalImage);
  auto largeImage = PB::Process::clone(originalImage);

  cv::resize(*smallImage, *smallImage, smallSize);
  cv::resize(*mediumImage, *mediumImage, mediumSize);
  cv::resize(*largeImage, *largeImage, largeSize);

  PB::infra::writeImageOnDisk(smallImage, small);
  PB::infra::writeImageOnDisk(mediumImage, medium);
  PB::infra::writeImageOnDisk(largeImage, large);

  return hash;
}

std::string ThumbnailsTask::createThumbnailsByPath(
    Path originalPath, std::shared_ptr<PlatformInfo> platformInfo,
    std::shared_ptr<IdentifyableProject> project, std::string targetHash)
{
  auto projectId = project->first;

  auto [large, medium, small, hash] =
      platformInfo->newThumbnailPaths(projectId, targetHash);

  ImageMetadataInspector imageMetadataInspector(originalPath);

  imageMetadataInspector.inspect();

  auto width = imageMetadataInspector.width();
  auto height = imageMetadataInspector.height();

  auto [largeSize, mediumSize, smallSize] = Geometry::compute3SizesGeometry(
      cv::Size{(int)width, (int)height},
      {(int)project->second.paperSettings.width,
       (int)project->second.paperSettings.height});

  auto originalImage = infra::loadImageToCvMat(originalPath);

  auto smallImage = PB::Process::clone(originalImage);
  auto mediumImage = PB::Process::clone(originalImage);
  auto largeImage = PB::Process::clone(originalImage);

  cv::resize(*smallImage, *smallImage, smallSize);
  cv::resize(*mediumImage, *mediumImage, mediumSize);
  cv::resize(*largeImage, *largeImage, largeSize);

  PB::infra::writeImageOnDisk(smallImage, small);
  PB::infra::writeImageOnDisk(mediumImage, medium);
  PB::infra::writeImageOnDisk(largeImage, large);

  return hash;
}

ThumbnailsTask::ThumbnailsTask(Path originalPath) : mOriginalPath(originalPath)
{
}

void ThumbnailsTask::configurePlatformInfo(
    std::shared_ptr<PlatformInfo> platformInfo)
{
  mPlatformInfo = platformInfo;
}

void ThumbnailsTask::configureProject(
    std::shared_ptr<IdentifyableProject> project)
{
  mProject = project;
}

std::string ThumbnailsTask::createThumbnails()
{
  return createThumbnailsByPath(mOriginalPath, mPlatformInfo, mProject);
}

} // namespace PB
