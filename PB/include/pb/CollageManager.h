#pragma once

#include <vector>

#include <pb/CollageLibraryAssistant.h>
#include <pb/CollageMakerJob.h>
#include <pb/CollageThumbnailsMakerJob.h>
#include <pb/DrawingService.h>
#include <pb/SVGInflater.h>
#include <pb/TaskCruncher.h>
#include <pb/project/Project.h>

namespace PB {
class CollageManager {
public:
  explicit CollageManager(Path localStatePath, Path installationPath);
  ~CollageManager() = default;

  void configureListener(CollageThumbnailsMakerListener *listener);
  void configureProject(std::shared_ptr<PB::Project> project);
  void configureProjectId(std::string projectId);

  void setTaskCruncher(std::shared_ptr<TaskCruncher> taskCruncher);
  void generateTemplatesImages();
  void combineImages(Path                           templatePath,
                     std::vector<std::vector<Path>> imagesPaths);

  std::vector<Path> getTemplatesPaths() const;

private:
  CollageThumbnailsMakerJob     mJob;
  CollageMakerJob               mCollageMakerJob;
  std::shared_ptr<TaskCruncher> mTaskCruncher = nullptr;

  std::vector<Path> getTemplatesPaths(Path directoryPath);
};
} // namespace PB
