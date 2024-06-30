#pragma once

#include <vector>

#include <pb/CollageLibraryAssistant.h>
#include <pb/DrawingService.h>
#include <pb/MapReducer.h>
#include <pb/SVGInflater.h>
#include <pb/project/Project.h>

namespace PB {

class CollageThumbnailsMakerListener {
public:
  virtual ~CollageThumbnailsMakerListener() = default;

  virtual void onThumbnailsCreated() = 0;
  virtual void onCollageThumbnailsMakerError() = 0;

private:
};

class CollageThumbnailsMakerJob final : public PB::MapReducer {
public:
  ~CollageThumbnailsMakerJob() = default;

  void configureListener(CollageThumbnailsMakerListener *listener);

  void mapTasks();

  std::optional<IdentifyableFunction>
  getNext(std::stop_token stopToken) override;

  void onFinished(PBDev::MapReducerTaskId) override;

private:
  static constexpr const char *COLLAGES_TEMPLATES_RESOURCES_NAME =
      "collages-templates-resources";

  static constexpr const char *COLLAGES_TEMPLATES_NAME = "svg-templates";

  CollageThumbnailsMakerListener *mListener = nullptr;

  Path                              mCollagesTemplatesResourcesPath;
  Path                              mInstallPath;
  CollageLibraryAssistant           mAssistant;
  std::shared_ptr<SkiaResources>    mResources = nullptr;
  DrawingService                    mDrawingService;
  std::shared_ptr<Project>          mProject = nullptr;
  std::vector<Path>                 mGeneratedLibraries;
  PBDev::SkiaResourcesId            mResourcesProviderId;
  std::vector<Path>                 mNumberedImages;
  std::vector<Path>                 mSourceTemplates;
  std::vector<Path>                 mProcessedSVGPaths;
  std::vector<IdentifyableFunction> mFunctions;

  unsigned mIndex = 0;

  void mapJobs();

  std::vector<Path> getTemplatesPaths(Path directoryPath);

  void createPlaceholdersFolder();

  void obtainSourceTemplates();

  void createNumberedPlaceholders();

  void createCustomSVGTemplate(unsigned i);

  void registerNewResource();

  void createTemplatesThumbnail(unsigned i);
};
} // namespace PB
