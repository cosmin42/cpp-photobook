#pragma once

#include <vector>

#include <pb/components/CollageLibraryAssistant.h>
#include <pb/components/MapReducer.h>
#include <pb/components/SVGInflater.h>
#include <pb/entities/CollageTemplateInfo.h>
#include <pb/project/Project.h>
#include <pb/services/DrawingService.h>

namespace PB {

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
  void configureProject(std::shared_ptr<Project> project);
  void configurePlatformInfo(std::shared_ptr<PlatformInfo> platformInfo);

  void mapJobs();

  std::vector<CollageTemplateInfo> getTemplatesPaths() const;
  std::vector<CollageTemplateInfo> getSourceTemplates() const;

  std::optional<IdentifyableFunction>
  getTask(std::stop_token stopToken) override;

  void onTaskFinished(PBDev::MapReducerTaskId) override;

  unsigned taskCount() const override { return (unsigned)mFunctions.size(); }

private:
  static constexpr const char *COLLAGES_TEMPLATES_RESOURCES_NAME =
      "collages-templates-resources";
  static constexpr const char *COLLAGES_TEMPLATES_NAME = "svg-templates";

  CollageThumbnailsMakerListener *mListener = nullptr;
  std::shared_ptr<PlatformInfo>   mPlatformInfo = nullptr;

  Path                                     mCollagesTemplatesResourcesPath;
  std::shared_ptr<CollageLibraryAssistant> mAssistant = nullptr;
  std::shared_ptr<SkiaResources>           mResources = nullptr;
  DrawingService                           mDrawingService;
  std::shared_ptr<Project>                 mProject = nullptr;
  std::vector<CollageTemplateInfo>         mGeneratedLibraries;
  PBDev::SkiaResourcesId                   mResourcesProviderId;
  std::vector<Path>                        mNumberedImages;
  std::vector<CollageTemplateInfo>         mSourceTemplates;
  std::vector<Path>                        mProcessedSVGPaths;
  std::vector<IdentifyableFunction>        mFunctions;

  unsigned mIndex = 0;

  Path collagesResourcePath();

  static CollageTemplateInfo parseTemplatePath(Path path);

  std::vector<CollageTemplateInfo> getTemplatesPaths(Path directoryPath);

  void createPlaceholdersFolder();

  void obtainSourceTemplates();

  void createNumberedPlaceholders();

  void createCustomSVGTemplate(unsigned i);

  void registerNewResource();

  void createTemplatesThumbnail(unsigned i);
};
} // namespace PB
