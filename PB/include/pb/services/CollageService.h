#pragma once

#include <vector>

#include <pb/components/CollageLibraryAssistant.h>
#include <pb/components/SVGInflater.h>
#include <pb/components/TaskCruncher.h>
#include <pb/jobs/CollageMakerJob.h>
#include <pb/jobs/CollageThumbnailsMakerJob.h>
#include <pb/project/Project.h>
#include <pb/services/DrawingService.h>

namespace PB {
class CollageService {
public:
  explicit CollageService();
  ~CollageService() = default;

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
