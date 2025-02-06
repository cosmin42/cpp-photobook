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

void CollageService::configureProject(
    std::shared_ptr<IdentifyableProject> project)
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

void CollageService::configureDurableHashService(
    std::shared_ptr<DurableHashService> durableHashService)
{
  mCollageMakerJob->configureDurableHashService(durableHashService);
}

void CollageService::configureImageFactory(
    std::shared_ptr<ImageFactory> imageFactory)
{
  mCollageMakerJob->configureImageFactory(imageFactory);
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

  mCollageMakerJob->mapJobs(templatePaths.at(templateIndex).path, imagesPaths);
  mCollageMakerStopSource =
      mTaskCruncher->crunch("collage-thumbnails", *mCollageMakerJob,
                            PBDev::ProgressJobName{"collage-combine"});
}

void CollageService::combineImages(unsigned                     templateIndex,
                                   std::vector<GenericImagePtr> images)
{
  auto templatePaths = mThumbnailsJob->getSourceTemplates();
  std::vector<Path> imagesPaths;
  for (const auto &image : images) {
    imagesPaths.push_back(image->full());
  }
  mCollageMakerJob->mapJobs(templatePaths.at(templateIndex).path, imagesPaths);
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
