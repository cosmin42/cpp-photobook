#pragma once

#include <pb/Platform.h>
#include <pb/components/OGLEngine.h>
#include <pb/entities/GenericImage.h>
#include <pb/entities/LutIconInfo.h>
#include <pb/image/ImageFactory.h>
#include <pb/infra/ThreadScheduler.h>
#include <pb/jobs/LutIconsPreprocessingJob.h>
#include <pb/services/DirectoryInspectionService.h>
#include <pb/services/DurableHashService.h>

DECLARE_STRONG_STRING(LutName)

DECLARE_STRONG_UUID(LutApplicationId)

namespace PB::Service {
class LutServiceListener {
public:
  virtual void onLutAdded(LutIconInfo iconInfo) = 0;
  virtual void onLutApplied(PBDev::LutApplicationId, GenericImagePtr) = 0;
  virtual void onLutAppliedInMemory(PBDev::LutApplicationId,
                                    std::shared_ptr<cv::Mat>) = 0;
  virtual void onLutAppliedOnDiskInplace(PBDev::LutApplicationId) = 0;
  virtual void onLutAppliedOnDisk(PBDev::LutApplicationId, GenericImagePtr) = 0;
};

class LutService final : public DirectoryInspectionJobListener,
                         public LutIconsPreprocessingListener {
public:
  LutService();
  ~LutService() = default;

  void configurePlatformInfo(std::shared_ptr<PlatformInfo> platformInfo);
  void configureTaskCruncher(std::shared_ptr<TaskCruncher> taskCruncher);
  void configureThreadScheduler(PBDev::ThreadScheduler *threadScheduler);
  void configureOGLEngine(std::shared_ptr<OGLEngine> oglEngine)
  {
    mOglEngine = oglEngine;
    mLutIconsPreprocessingJob.configureOGLEngine(oglEngine);
  }

  void configureDurableHashService(
      std::shared_ptr<DurableHashService> durableHashService)
  {
    mDurableHashService = durableHashService;
  }

  void configureProject(std::shared_ptr<IdentifyableProject> project);

  void configureImageFactory(std::shared_ptr<ImageFactory> imageFactory);

  void configureLutServiceListener(LutServiceListener *listener);

  void startLutService();
  void detectLuts();

  bool isRunning() const;

  void stop();

  void onInspectionFinished(PBDev::DirectoryInspectionJobId id,
                            std::vector<Path> searchResults) override;

  void onLutIconsPreprocessingFinished(std::string lutName, Path cubeFile,
                                       Path icon) override;

  void applyLut(PBDev::LutApplicationId lutId, unsigned lutIndex,
                GenericImagePtr image);

  void applyLutInMemory(PBDev::LutApplicationId lutId, unsigned lutIndex,
                        cv::Mat image);

  void applyTransformationOnDisk(PBDev::LutApplicationId lutId,
                                 unsigned lutIndex, GenericImagePtr image,
                                 bool inplace = true);

  void applyLutAndEffects(PBDev::LutApplicationId lutId, unsigned lutIndex,
                          GenericImagePtr image, double saturation,
                          double contrast, double brightness);

  std::vector<LutIconInfo> listLuts() const;

private:
  static constexpr const char *IMAGE_NAME = "singapore.png";
  static constexpr const char *FOLDER_NAME = "others";

  std::shared_ptr<DurableHashService>         mDurableHashService = nullptr;
  std::shared_ptr<PlatformInfo>               mPlatformInfo = nullptr;
  std::shared_ptr<DirectoryInspectionService> mDirectoryInspectionService =
      nullptr;
  std::shared_ptr<TaskCruncher> mTaskCruncher = nullptr;
  std::shared_ptr<OGLEngine>    mOglEngine = nullptr;
  PBDev::ThreadScheduler       *mThreadScheduler = nullptr;
  LutServiceListener           *mLutServiceListener = nullptr;

  std::stop_source mLutCreationStopSource;

  std::unordered_map<PBDev::LutApplicationId, std::string,
                     boost::hash<PBDev::LutApplicationId>>
      mOutImageHashes;

  PBDev::DirectoryInspectionJobId mLutsInspectionId =
      PBDev::DirectoryInspectionJobId(RuntimeUUID::newUUID());

  LutIconsPreprocessingJob mLutIconsPreprocessingJob;

  std::vector<LutIconInfo> mLutsIconsInfo;
  std::vector<Path>        mLutsPathsToBeProcessed;
  std::vector<Path>        mLutsPaths;

  std::shared_ptr<IdentifyableProject> mProject = nullptr;

  std::shared_ptr<ImageFactory> mImageFactory = nullptr;

  Path originalImagePath() const
  {
    return mPlatformInfo->installationPath / FOLDER_NAME / IMAGE_NAME;
  }

  Path lutAssetsPath() const;

  bool lutExists(const Path &path) const;
};
} // namespace PB::Service
