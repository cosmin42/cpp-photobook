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

std::vector<CollageTemplateInfo>
CollageManager::getTemplatesPaths(Path directoryPath)
{
  std::vector<CollageTemplateInfo> collageTemplatesInfo;

  for (const auto &entry : std::filesystem::directory_iterator(directoryPath)) {
    if (entry.is_regular_file()) {
      auto collageTemplateInfo = parseTemplatePath(entry.path());
      collageTemplatesInfo.push_back(collageTemplateInfo);
    }
  }

  return collageTemplatesInfo;
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

CollageTemplateInfo CollageManager::parseTemplatePath(Path path)
{
  CollageTemplateInfo collageTemplateInfo;

  collageTemplateInfo.path = path;
  collageTemplateInfo.name = path.stem().string();
  collageTemplateInfo.name =
      std::regex_replace(collageTemplateInfo.name, std::regex("-"), " ");
  collageTemplateInfo.name =
      std::regex_replace(collageTemplateInfo.name, std::regex("_"), " ");

  std::string imageCountStr =
      collageTemplateInfo.name.substr(0, collageTemplateInfo.name.find('-'));

  try {
    collageTemplateInfo.imageCount = std::stoi(imageCountStr);
  }
  catch (std::invalid_argument &) {
    PBDev::basicAssert(false);
  }
  return collageTemplateInfo;
}

} // namespace PB
