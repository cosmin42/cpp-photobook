#pragma once

#include <vector>

#include <pb/components/CollageLibraryAssistant.h>
#include <pb/components/SVGInflater.h>
#include <pb/infra/TaskCruncher.h>
#include <pb/jobs/CollageMakerJob.h>
#include <pb/jobs/CollageThumbnailsMakerJob.h>
#include <pb/components/Project.h>
#include <pb/services/DrawingService.h>

using namespace PB::Job;

namespace PB::Service {
class CollageService {
public:
  explicit CollageService();
  ~CollageService() = default;

  void configureThumbnailsListener(CollageThumbnailsMakerListener *listener);
  void configureCollageMakerListener(CollageMakerListener *listener);

  void configureProject(std::shared_ptr<IdentifyableProject> project);
  void configurePlatformInfo(std::shared_ptr<PlatformInfo> platformInfo);
  void configureTaskCruncher(std::shared_ptr<TaskCruncher> taskCruncher);
  void configureDurableHashService(
      std::shared_ptr<DurableHashService> durableHashService);
  void configureImageFactory(std::shared_ptr<ImageFactory> imageFactory);

  void configureVulkanManager(std::shared_ptr<VulkanManager> vulkanManager);

  void generateTemplatesImages();
  void combineImages(unsigned templateIndex, std::vector<Path> imagesPaths);

  void combineImages(unsigned                     templateIndex,
                     std::vector<GenericImagePtr> images);

  std::vector<CollageTemplateInfo> getTemplatesPaths() const;

  bool isRunning() const;
  void stop();
  void clean();

private:
  std::stop_source                           mThumbnailsMakerStopSource;
  std::stop_source                           mCollageMakerStopSource;
  std::shared_ptr<CollageThumbnailsMakerJob> mThumbnailsJob = nullptr;
  std::shared_ptr<CollageMakerJob>           mCollageMakerJob = nullptr;
  std::shared_ptr<TaskCruncher>              mTaskCruncher = nullptr;
  std::shared_ptr<PlatformInfo>              mPlatformInfo = nullptr;
  std::shared_ptr<DurableHashService>        mDurableHashService = nullptr;
};
} // namespace PB
