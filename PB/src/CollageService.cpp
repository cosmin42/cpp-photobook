#include <pb/services/CollageService.h>

#pragma warning(push)
#pragma warning(disable : 4996)
#include <spdlog/spdlog.h>
#pragma warning(pop)

#include <regex>

namespace PB::Service {
CollageService::CollageService()
    : mThumbnailsJob(std::make_shared<CollageThumbnailsMakerJob>()),
      mCollageMakerJob(std::make_shared<CollageMakerJob>())
{
}

void CollageService::configureThumbnailsListener(
    CollageThumbnailsMakerListener *listener)
{
  mThumbnailsJob->configureListener(listener);
}

void CollageService::configureCollageMakerListener(
    CollageMakerListener *listener)
{
  mCollageMakerJob->configureListener(listener);
}

void CollageService::configureProject(IdentifiableProject project)
{
  mThumbnailsJob->configureProject(project);
  mCollageMakerJob->configureProject(project);
}

void CollageService::configurePlatformInfo(
    std::shared_ptr<PlatformInfo> platformInfo)
{
  mPlatformInfo = platformInfo;
  mThumbnailsJob->configurePlatformInfo(platformInfo);
  mCollageMakerJob->configurePlatformInfo(platformInfo);
}

void CollageService::configureTaskCruncher(
    std::shared_ptr<TaskCruncher> taskCruncher)
{
  mTaskCruncher = taskCruncher;
}

void CollageService::configureImageFactory(
    std::shared_ptr<ImageFactory> imageFactory)
{
  mCollageMakerJob->configureImageFactory(imageFactory);
}

void CollageService::configureVulkanManager(
    std::shared_ptr<VulkanManager> vulkanManager)
{
  mThumbnailsJob->configureVulkanManager(vulkanManager);
  mCollageMakerJob->configureVulkanManager(vulkanManager);
}

void CollageService::generateTemplatesImages()
{
  mThumbnailsJob->mapJobs();
  mThumbnailsMakerStopSource =
      mTaskCruncher->crunch("collage-thumbnails", *mThumbnailsJob,
                            PBDev::ProgressJobName{"collages-gen"});
}

std::vector<CollageTemplateInfo> CollageService::getTemplatesPaths() const
{
  return mThumbnailsJob->getTemplatesPaths();
}

void CollageService::combineImages(unsigned          templateIndex,
                                   std::vector<Path> imagesPaths)
{
  auto templatePaths = mThumbnailsJob->getSourceTemplates();

  std::string accumulatedName;
  for (const auto &imagePath : imagesPaths) {
    accumulatedName += imagePath.filename().string() + ";";
  }

  mCollageMakerJob->mapJobs(templatePaths.at(templateIndex).path, imagesPaths, accumulatedName);
  mCollageMakerStopSource =
      mTaskCruncher->crunch("collage-thumbnails", *mCollageMakerJob,
                            PBDev::ProgressJobName{"collage-combine"});
}

void CollageService::combineImages(unsigned                     templateIndex,
                                   std::vector<GenericImagePtr> images)
{
  auto              templatePaths = mThumbnailsJob->getSourceTemplates();
  std::vector<Path> imagesPaths;
  std::vector<std::string> imagesNames;
  for (const auto &image : images) {
    imagesNames.push_back(image->name());
  }
  for (const auto &image : images) {
    imagesPaths.push_back(Path("thumbnail-images") / image->full());
  }

  std::string accumulatedName;
  for (const auto &image : images) {
    accumulatedName += image->name() + ";";
  }

  mCollageMakerJob->mapJobs(templatePaths.at(templateIndex).path, imagesPaths, accumulatedName);
  mCollageMakerStopSource =
      mTaskCruncher->crunch("collage-thumbnails", *mCollageMakerJob,
                            PBDev::ProgressJobName{"collage-combine"});
}

bool CollageService::isRunning() const
{
  auto isThumbnailsJobRunning = !mThumbnailsJob->isFinished();
  auto isCollageMakerJobRunning = !mCollageMakerJob->isFinished();
  return isThumbnailsJobRunning || isCollageMakerJobRunning;
}

void CollageService::stop()
{
  mThumbnailsMakerStopSource.request_stop();
  mCollageMakerStopSource.request_stop();
}

void CollageService::clean()
{
  mThumbnailsMakerStopSource = std::stop_source();
  mCollageMakerStopSource = std::stop_source();

  mCollageMakerJob = std::make_shared<CollageMakerJob>();
  mThumbnailsJob = std::make_shared<CollageThumbnailsMakerJob>();
}

} // namespace PB::Service
