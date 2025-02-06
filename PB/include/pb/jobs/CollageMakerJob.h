#pragma once

#include <pb/components/CollageLibraryAssistant.h>
#include <pb/components/MapReducer.h>
#include <pb/components/SkiaResources.h>
#include <pb/image/ImageFactory.h>
#include <pb/project/Project.h>
#include <pb/services/DrawingService.h>
#include <pb/services/DurableHashService.h>

namespace PB::Job {
class CollageMakerListener {
public:
  virtual ~CollageMakerListener() = default;

  virtual void onCollageCreated(GenericImagePtr aggregatedImage) = 0;
  virtual void onCollageMakerError() = 0;

private:
};

class CollageMakerJob final : public MapReducer {
public:
  CollageMakerJob();
  ~CollageMakerJob() = default;

  void configureListener(CollageMakerListener *listener);
  void configureProject(std::shared_ptr<IdentifyableProject> project);
  void configurePlatformInfo(std::shared_ptr<PlatformInfo> platformInfo);
  void configureDurableHashService(
      std::shared_ptr<DurableHashService> durableHashService);
  void configureImageFactory(std::shared_ptr<ImageFactory> imageFactory);

  void mapJobs(Path templatePath, std::vector<Path> imagesPaths);

  std::optional<IdentifyableFunction>
  getTask(std::stop_token stopToken) override;

  void onTaskFinished(PBDev::MapReducerTaskId) override;

  unsigned taskCount() const override { return (unsigned)mFunctions.size(); }

private:
  static constexpr const char *COLLAGES_TEMPLATES_NAME = "svg-templates";
  static constexpr const char *TEMPORARY_SVG_FILE_NAME =
      "temporary-svg-file-name.svg";

  CollageMakerListener                    *mListener = nullptr;
  std::shared_ptr<PlatformInfo>            mPlatformInfo = nullptr;
  std::shared_ptr<CollageLibraryAssistant> mAssistant = nullptr;
  std::shared_ptr<IdentifyableProject>     mProject = nullptr;
  std::shared_ptr<SkiaResources>           mResources = nullptr;
  std::shared_ptr<DurableHashService>      mDurableHashService = nullptr;
  std::shared_ptr<ImageFactory>            mImageFactory = nullptr;
  Service::DrawingService                  mDrawingService;
  PBDev::SkiaResourcesId                   mResourcesProviderId;
  std::vector<IdentifyableFunction>        mFunctions;

  std::unordered_map<PBDev::MapReducerTaskId, Path,
                     boost::hash<PBDev::MapReducerTaskId>>
      mCollagePath;

  unsigned mIndex = 0;
};
} // namespace PB::Job
