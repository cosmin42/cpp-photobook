#pragma once

#include <vector>

#include <pb/CollageLibraryAssistant.h>
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

  void setTaskCruncher(std::shared_ptr<TaskCruncher> taskCruncher);

  void generateTemplatesImages();

  std::vector<Path> getTemplatesPaths() const;

private:
  CollageThumbnailsMakerJob     mJob;
  std::shared_ptr<TaskCruncher> mTaskCruncher = nullptr;

  std::vector<Path> getTemplatesPaths(Path directoryPath);
};
} // namespace PB
