#include <pb/CollageManager.h>

#pragma warning(push)
#pragma warning(disable : 4996)
#include <spdlog/spdlog.h>
#pragma warning(pop)

#include <regex>

namespace PB {
CollageManager::CollageManager()
    : mJob(std::make_shared<CollageThumbnailsMakerJob>()),
      mCollageMakerJob(std::make_shared<CollageMakerJob>())
{
}

void CollageManager::configureListener(CollageThumbnailsMakerListener *listener)
{
  mJob->configureListener(listener);
}

void CollageManager::configureCollageMakerListener(
    CollageMakerListener *listener)
{
  mCollageMakerJob->configureListener(listener);
}

void CollageManager::configureProject(std::shared_ptr<PB::Project> project)
{
  mJob->configureProject(project);
  mCollageMakerJob->configureProject(project);
}

void CollageManager::configureProjectId(std::string projectId)
{
  mCollageMakerJob->configureProjectId(projectId);
}

void CollageManager::configurePlatformInfo(
    std::shared_ptr<PlatformInfo> platformInfo)
{
  mPlatformInfo = platformInfo;
  mJob->configurePlatformInfo(platformInfo);
  mCollageMakerJob->configurePlatformInfo(platformInfo);
}

void CollageManager::setTaskCruncher(std::shared_ptr<TaskCruncher> taskCruncher)
{
  mTaskCruncher = taskCruncher;
}

void CollageManager::generateTemplatesImages()
{
  mJob->mapJobs();
  mTaskCruncher->crunch("collage-thumbnails", *mJob,
                        PBDev::ProgressJobName{"collages-gen"});
}

std::vector<CollageTemplateInfo> CollageManager::getTemplatesPaths() const
{
  return mJob->getTemplatesPaths();
}

void CollageManager::combineImages(unsigned          templateIndex,
                                   std::vector<Path> imagesPaths)
{
  auto templatePaths = mJob->getSourceTemplates();

  mCollageMakerJob->mapJobs(templatePaths.at(templateIndex).path, imagesPaths);
  mTaskCruncher->crunch("collage-thumbnails", *mCollageMakerJob,
                        PBDev::ProgressJobName{"collage-combine"});
}

} // namespace PB
