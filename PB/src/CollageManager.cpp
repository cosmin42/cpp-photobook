#include <pb/CollageManager.h>

#pragma warning(push)
#pragma warning(disable : 4996)
#include <spdlog/spdlog.h>
#pragma warning(pop)

#include <regex>

namespace PB {
CollageManager::CollageManager(Path localStatePath, Path installPath)
    : mJob(localStatePath, installPath),
      mCollageMakerJob(localStatePath, installPath)
{
}

void CollageManager::configureListener(CollageThumbnailsMakerListener *listener)
{
  mJob.configureListener(listener);
}

void CollageManager::configureCollageMakerListener(
    CollageMakerListener *listener)
{
  mCollageMakerJob.configureListener(listener);
}

void CollageManager::configureProject(std::shared_ptr<PB::Project> project)
{
  mJob.configureProject(project);
  mCollageMakerJob.configureProject(project);
}

void CollageManager::configureProjectId(std::string projectId)
{
  mCollageMakerJob.configureProjectId(projectId);
}

void CollageManager::setTaskCruncher(std::shared_ptr<TaskCruncher> taskCruncher)
{
  mTaskCruncher = taskCruncher;
}

void CollageManager::generateTemplatesImages()
{
  mJob.mapJobs();
  mTaskCruncher->crunch("collage-thumbnails", mJob);
}

std::vector<CollageTemplateInfo> CollageManager::getTemplatesPaths() const
{
  return mJob.getTemplatesPaths();
}

void CollageManager::combineImages(unsigned          templateIndex,
                                   std::vector<Path> imagesPaths)
{
  auto templatePaths = mJob.getSourceTemplates();

  mCollageMakerJob.mapJobs(templatePaths.at(templateIndex).path, imagesPaths);
  mTaskCruncher->crunch("collage-thumbnails", mCollageMakerJob);
}

} // namespace PB
