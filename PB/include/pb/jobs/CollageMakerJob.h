#pragma once

#include <pb/components/CollageLibraryAssistant.h>
#include <pb/components/MapReducer.h>
#include <pb/components/Project.h>
#include <pb/components/SkiaResources.h>
#include <pb/image/ImageFactory.h>
#include <pb/services/DrawingService.h>

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
  void configureProject(IdentifiableProject project);
  void configurePlatformInfo(std::shared_ptr<PlatformInfo> platformInfo);
  void configureImageFactory(std::shared_ptr<ImageFactory> imageFactory);
  void configureVulkanManager(std::shared_ptr<VulkanManager> vulkanManager);

  void mapJobs(Path templatePath, std::vector<Path> imagesPaths, std::string newImageName);

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
  IdentifiableProject                      mProject = nullptr;
  std::shared_ptr<SkiaResources>           mResources = nullptr;
  std::shared_ptr<ImageFactory>            mImageFactory = nullptr;
  Service::DrawingService                  mDrawingService;
  PBDev::SkiaResourcesId                   mResourcesProviderId;
  std::vector<IdentifyableFunction>        mFunctions;

  std::unordered_map<PBDev::MapReducerTaskId, Path,
                     boost::hash<PBDev::MapReducerTaskId>>
      mCollagePath;

  std::vector<Path> mSources;
  std::string mNewImageName;

  unsigned mIndex = 0;
};
} // namespace PB::Job
