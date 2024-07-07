#include <pb/CollageTemplatesManager.h>

#pragma warning(push)
#pragma warning(disable : 4996)
#include <spdlog/spdlog.h>
#pragma warning(pop)

namespace PB {
CollageTemplatesManager::CollageTemplatesManager(Path localStatePath,
                                                 Path installPath)
    : mJob(localStatePath, installPath)
{
}

void CollageTemplatesManager::configureListener(
    CollageThumbnailsMakerListener *listener)
{
  mJob.configureListener(listener);
}

void CollageTemplatesManager::configureProject(
    std::shared_ptr<PB::Project> project)
{
  mJob.configureProject(project);
}

void CollageTemplatesManager::setTaskCruncher(
    std::shared_ptr<TaskCruncher> taskCruncher)
{
  mTaskCruncher = taskCruncher;
}

std::vector<Path> CollageTemplatesManager::getTemplatesPaths(Path directoryPath)
{
  std::vector<Path> templatesPaths;

  for (const auto &entry : std::filesystem::directory_iterator(directoryPath)) {
    if (entry.is_regular_file()) {
      templatesPaths.push_back(entry.path());
    }
  }

  return templatesPaths;
}

void CollageTemplatesManager::generateTemplatesImages()
{
  mJob.mapJobs();
  mTaskCruncher->crunch("collage-thumbnails", mJob);
}

std::vector<Path> CollageTemplatesManager::getTemplatesPaths() const
{
  return mJob.getTemplatesPaths();
}

} // namespace PB
