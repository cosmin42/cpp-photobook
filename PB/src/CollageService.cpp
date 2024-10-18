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

void CollageService::configureThumbnailsListener(CollageThumbnailsMakerListener *listener)
{
  mThumbnailsJob->configureListener(listener);
}

void CollageService::configureCollageMakerListener(
    CollageMakerListener *listener)
{
  mCollageMakerJob->configureListener(listener);
}

void CollageService::configureProject(std::shared_ptr<PB::Project> project)
{
  mThumbnailsJob->configureProject(project);
  mCollageMakerJob->configureProject(project);
}

void CollageService::configureProjectId(std::string projectId)
{
  mCollageMakerJob->configureProjectId(projectId);
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

void CollageService::generateTemplatesImages()
{
  mThumbnailsJob->mapJobs();
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
  mTaskCruncher->crunch("collage-thumbnails", *mCollageMakerJob,
                        PBDev::ProgressJobName{"collage-combine"});
}

} // namespace PB::Service
