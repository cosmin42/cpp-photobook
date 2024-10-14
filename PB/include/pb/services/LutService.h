#pragma once

#include <pb/Platform.h>
#include <pb/components/OGLEngine.h>
#include <pb/components/ThreadScheduler.h>
#include <pb/entities/LutIconInfo.h>
#include <pb/jobs/LutIconsPreprocessingJob.h>
#include <pb/services/DirectoryInspectionService.h>

DECLARE_STRONG_STRING(LutName)

namespace PB::Service {
class LutServiceListener {
public:
  virtual void onLutAdded(LutIconInfo iconInfo) = 0;
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

  void configureLutServiceListener(LutServiceListener *listener);

  void startLutService();
  void detectLuts();

  void onInspectionFinished(PBDev::DirectoryInspectionJobId id,
                            std::vector<Path> searchResults) override;

  void onLutIconsPreprocessingFinished(std::string lutName, Path icon) override;

  std::vector<LutIconInfo> listLuts() const;

private:
  static constexpr const char *IMAGE_NAME = "singapore.jpg";
  static constexpr const char *FOLDER_NAME = "others";

  std::shared_ptr<PlatformInfo>               mPlatformInfo = nullptr;
  std::shared_ptr<DirectoryInspectionService> mDirectoryInspectionService =
      nullptr;
  std::shared_ptr<TaskCruncher> mTaskCruncher = nullptr;
  std::shared_ptr<OGLEngine>    mOglEngine = nullptr;
  PBDev::ThreadScheduler       *mThreadScheduler = nullptr;
  LutServiceListener           *mLutServiceListener = nullptr;

  PBDev::DirectoryInspectionJobId mLutsInspectionId =
      PBDev::DirectoryInspectionJobId(RuntimeUUID::newUUID());

  LutIconsPreprocessingJob mLutIconsPreprocessingJob;

  std::vector<LutIconInfo> mLutsPaths;

  Path originalImagePath() const
  {
    return mPlatformInfo->installationPath / FOLDER_NAME / IMAGE_NAME;
  }

  Path lutAssetsPath() const;
};
} // namespace PB
