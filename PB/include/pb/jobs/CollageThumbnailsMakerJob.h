#pragma once

#include <vector>

#include <pb/components/CollageLibraryAssistant.h>
#include <pb/components/MapReducer.h>
#include <pb/components/Project.h>
#include <pb/components/SVGInflater.h>
#include <pb/entities/CollageTemplateInfo.h>
#include <pb/services/DrawingService.h>

using namespace PB::Service;

namespace PB::Job {

class CollageThumbnailsMakerListener {
public:
  virtual ~CollageThumbnailsMakerListener() = default;

  virtual void onThumbnailsCreated() = 0;
  virtual void onCollageThumbnailsMakerError() = 0;

private:
};

class CollageThumbnailsMakerJob final : public MapReducer {
public:
  CollageThumbnailsMakerJob();
  ~CollageThumbnailsMakerJob() = default;

  void configureListener(CollageThumbnailsMakerListener *listener);
  void configureProject(std::shared_ptr<IdentifyableProject> project);
  void configurePlatformInfo(std::shared_ptr<PlatformInfo> platformInfo);
  void configureVulkanManager(std::shared_ptr<VulkanManager> vulkanManager);

  void mapJobs();

  std::vector<CollageTemplateInfo> getTemplatesPaths() const;
  std::vector<CollageTemplateInfo> getSourceTemplates() const;

  std::optional<IdentifyableFunction>
  getTask(std::stop_token stopToken) override;

  void onTaskFinished(PBDev::MapReducerTaskId) override;

  unsigned taskCount() const override { return (unsigned)mFunctions.size(); }

private:
  static constexpr const char *COLLAGES_TEMPLATES_NAME = "svg-templates";

  CollageThumbnailsMakerListener *mListener = nullptr;
  std::shared_ptr<PlatformInfo>   mPlatformInfo = nullptr;

  Path                                     mCollagesTemplatesResourcesPath;
  std::shared_ptr<CollageLibraryAssistant> mAssistant = nullptr;
  std::shared_ptr<SkiaResources>           mResources = nullptr;
  DrawingService                           mDrawingService;
  std::shared_ptr<IdentifyableProject>     mProject = nullptr;
  std::vector<CollageTemplateInfo>         mGeneratedLibraries;
  PBDev::SkiaResourcesId                   mResourcesProviderId;
  std::vector<Path>                        mNumberedImages;
  std::vector<CollageTemplateInfo>         mSourceTemplates;
  std::vector<Path>                        mProcessedSVGPaths;
  std::vector<IdentifyableFunction>        mFunctions;

  unsigned mIndex = 0;

  static CollageTemplateInfo parseTemplatePath(Path path);

  std::vector<CollageTemplateInfo> getTemplatesPaths(Path directoryPath);

  void createPlaceholdersFolder();

  void obtainSourceTemplates();

  void createNumberedPlaceholders();

  void createCustomSVGTemplate(unsigned i);

  void registerNewResource();

  void createTemplatesThumbnail(unsigned i);
};
} // namespace PB::Job
