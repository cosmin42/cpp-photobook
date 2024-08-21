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
  explicit CollageManager();
  ~CollageManager() = default;

  void configureListener(CollageThumbnailsMakerListener *listener);
  void configureCollageMakerListener(CollageMakerListener *listener);
  void configureProject(std::shared_ptr<PB::Project> project);
  void configureProjectId(std::string projectId);
  void configurePlatformInfo(std::shared_ptr<PlatformInfo> platformInfo);

  void setTaskCruncher(std::shared_ptr<TaskCruncher> taskCruncher);
  void generateTemplatesImages();
  void combineImages(unsigned templateIndex, std::vector<Path> imagesPaths);

  std::vector<CollageTemplateInfo> getTemplatesPaths() const;

private:
  std::shared_ptr<CollageThumbnailsMakerJob> mJob = nullptr;
  std::shared_ptr<CollageMakerJob>           mCollageMakerJob = nullptr;
  std::shared_ptr<TaskCruncher>              mTaskCruncher = nullptr;
  std::shared_ptr<PlatformInfo>              mPlatformInfo = nullptr;
};
} // namespace PB
