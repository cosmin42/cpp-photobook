#pragma once

#include <pb/DirectoryInspectionService.h>
#include <pb/LutIconInfo.h>
#include <pb/LutIconsPreprocessingJob.h>
#include <pb/OGLEngine.h>
#include <pb/Platform.h>
#include <pb/ThreadScheduler.h>

DECLARE_STRONG_STRING(LutName)

namespace PB {
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
  void configureDirectoryInspectionService(
      std::shared_ptr<DirectoryInspectionService> directoryInspectionService);
  void configureTaskCruncher(std::shared_ptr<TaskCruncher> taskCruncher);
  void condifureThreadScheduler(PBDev::ThreadScheduler *threadScheduler);
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

  void onLutIconsPreprocessingFinished(Path icon) override;

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
