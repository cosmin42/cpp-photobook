#pragma once

#include <pb/CollageLibraryAssistant.h>
#include <pb/DrawingService.h>
#include <pb/MapReducer.h>
#include <pb/SkiaResources.h>
#include <pb/project/Project.h>

namespace PB {
class CollageMakerListener {
public:
  virtual ~CollageMakerListener() = default;

  virtual void onCollageCreated(unsigned index) = 0;
  virtual void onCollageMakerError() = 0;

private:
};

class CollageMakerJob final : public PB::MapReducer {
public:
  CollageMakerJob(Path localStatePath, Path installPath);
  ~CollageMakerJob() = default;

  void configureListener(CollageMakerListener *listener);
  void configureProject(std::shared_ptr<Project> project);
  void configureProjectId(std::string projectId);

  void mapJobs(Path templatePath, std::vector<std::vector<Path>> imagesPaths);

  std::optional<IdentifyableFunction>
  getNext(std::stop_token stopToken) override;

  void onFinished(PBDev::MapReducerTaskId) override;

private:
  static constexpr const char *COLLAGES_TEMPLATES_NAME = "svg-templates";

  CollageMakerListener *mListener = nullptr;

  Path                              mCollagesResourcesPath;
  Path                              mInstallPath;
  Path                              mLocalStatePath;
  CollageLibraryAssistant           mAssistant;
  std::shared_ptr<Project>          mProject = nullptr;
  std::shared_ptr<SkiaResources>    mResources = nullptr;
  DrawingService                    mDrawingService;
  PBDev::SkiaResourcesId            mResourcesProviderId;
  std::vector<IdentifyableFunction> mFunctions;
  std::string                       mProjectId;

  std::unordered_map<PBDev::MapReducerTaskId, unsigned,
                     boost::hash<PBDev::MapReducerTaskId>>
      mCollageIndex;

  unsigned mIndex = 0;
};
} // namespace PB
