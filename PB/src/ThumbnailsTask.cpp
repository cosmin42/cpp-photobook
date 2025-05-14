#include <pb/components/ThumbnailsTask.h>

#include <pb/components/ImageMetadataInspector.h>
#include <pb/infra/FileSupport.h>

namespace PB {

// TODO: reconcile with the other createThumbnails
std::string
ThumbnailsTask::createThumbnails(std::shared_ptr<cv::Mat>      originalImage,
                                 std::shared_ptr<PlatformInfo> platformInfo,
                                 std::shared_ptr<NoirMonitor> noirMonitor,
                                 IdentifiableProject           project,
                                 std::string                   targetHash)
{

  auto projectId = project->id;

  if (targetHash.empty()) {
    targetHash = "wait";
  }
  noirMonitor->start("Thumbnails Stage 1", targetHash);
  auto [large, medium, small, hash] =
      platformInfo->newThumbnailPaths(projectId, targetHash);

  auto width = originalImage->cols;
  auto height = originalImage->rows;

  auto [largeSize, mediumSize, smallSize] = Geometry::compute3SizesGeometry(
      cv::Size{(int)width, (int)height},
      {(int)project->value.paperSettings.width,
       (int)project->value.paperSettings.height});

  noirMonitor->stop("Thumbnails Stage 1", targetHash);
  noirMonitor->start("Thumbnails Stage 2", targetHash);

  auto smallImage = PB::Process::clone(originalImage);
  auto mediumImage = PB::Process::clone(originalImage);
  auto largeImage = PB::Process::clone(originalImage);

  noirMonitor->stop("Thumbnails Stage 2", targetHash);
  noirMonitor->start("Thumbnails Stage 3", targetHash);

  cv::resize(*smallImage, *smallImage, smallSize);
  cv::resize(*mediumImage, *mediumImage, mediumSize);
  cv::resize(*largeImage, *largeImage, largeSize);

  noirMonitor->stop("Thumbnails Stage 3", targetHash);
  noirMonitor->start("Thumbnails Stage 4", targetHash);

  PB::infra::writeImageOnDisk(smallImage, small);
  PB::infra::writeImageOnDisk(mediumImage, medium);
  PB::infra::writeImageOnDisk(largeImage, large);

  noirMonitor->stop("Thumbnails Stage 4", targetHash);

  return hash;
}

std::string ThumbnailsTask::createThumbnailsByPath(
    Path originalPath, std::shared_ptr<PlatformInfo> platformInfo,
    std::shared_ptr<NoirMonitor> noirMonitor,
    IdentifiableProject project, std::string targetHash)
{
  noirMonitor->start("Thumbnails Stage 1", targetHash);
  auto projectId = project->id;

  auto [large, medium, small, hash] =
      platformInfo->newThumbnailPaths(projectId, targetHash);

  ImageMetadataInspector imageMetadataInspector(originalPath);

  imageMetadataInspector.inspect();

  auto width = imageMetadataInspector.width();
  auto height = imageMetadataInspector.height();

  auto [largeSize, mediumSize, smallSize] = Geometry::compute3SizesGeometry(
      cv::Size{(int)width, (int)height},
      {(int)project->value.paperSettings.width,
       (int)project->value.paperSettings.height});

  noirMonitor->stop("Thumbnails Stage 1", targetHash);
  noirMonitor->start("Thumbnails Stage 2", targetHash);

  auto originalImage = infra::loadImageToCvMat(originalPath);

  auto smallImage = PB::Process::clone(originalImage);
  auto mediumImage = PB::Process::clone(originalImage);
  auto largeImage = PB::Process::clone(originalImage);

  noirMonitor->stop("Thumbnails Stage 2", targetHash);
  noirMonitor->start("Thumbnails Stage 3", targetHash);

  cv::resize(*smallImage, *smallImage, smallSize);
  cv::resize(*mediumImage, *mediumImage, mediumSize);
  cv::resize(*largeImage, *largeImage, largeSize);

  noirMonitor->stop("Thumbnails Stage 3", targetHash);
  noirMonitor->start("Thumbnails Stage 4", targetHash);

  PB::infra::writeImageOnDisk(smallImage, small);
  PB::infra::writeImageOnDisk(mediumImage, medium);
  PB::infra::writeImageOnDisk(largeImage, large);

  noirMonitor->stop("Thumbnails Stage 4", targetHash);

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

void ThumbnailsTask::configureProject(IdentifiableProject project)
{
  mProject = project;
}

void ThumbnailsTask::configureNoirMonitor(std::shared_ptr<NoirMonitor> noirMonitor)
{
  mNoirMonitor = noirMonitor;
}

std::string ThumbnailsTask::createThumbnails()
{
  return createThumbnailsByPath(mOriginalPath, mPlatformInfo, mNoirMonitor,
                                mProject);
}

} // namespace PB
