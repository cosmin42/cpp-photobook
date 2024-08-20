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

  virtual void onCollageCreated(unsigned index, Path imagePath) = 0;
  virtual void onCollageMakerError() = 0;

private:
};

class CollageMakerJob final : public MapReducer {
public:
  CollageMakerJob(Path localStatePath, Path installPath);
  ~CollageMakerJob() = default;

  void configureListener(CollageMakerListener *listener);
  void configureProject(std::shared_ptr<Project> project);
  void configureProjectId(std::string projectId);

  void mapJobs(Path templatePath, std::vector<Path> imagesPaths);

  std::optional<IdentifyableFunction>
  getTask(std::stop_token stopToken) override;

  void onTaskFinished(PBDev::MapReducerTaskId) override;

  unsigned taskCount() const override { return mFunctions.size(); }

private:
  static constexpr const char *COLLAGES_TEMPLATES_NAME = "svg-templates";
  static constexpr const char *TEMPORARY_SVG_FILE_NAME =
      "temporary-svg-file-name.svg";

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

  std::unordered_map<PBDev::MapReducerTaskId, Path,
                     boost::hash<PBDev::MapReducerTaskId>>
      mCollagePath;

  unsigned mIndex = 0;
};
} // namespace PB
