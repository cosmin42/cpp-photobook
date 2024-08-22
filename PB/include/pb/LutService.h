#pragma once

#include <pb/DirectoryInspectionService.h>
#include <pb/LutIconsPreprocessingJob.h>
#include <pb/Platform.h>
#include <pb/ThreadScheduler.h>

DECLARE_STRONG_STRING(LutName)

namespace PB {
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


  void startLutService();
  void detectLuts();

  void onInspectionFinished(PBDev::DirectoryInspectionJobId id,
                            std::vector<Path> searchResults) override;

  void onLutIconsPreprocessingFinished(Path icon) override {}

  std::unordered_set<PBDev::LutName, boost::hash<PBDev::LutName>>
  listLuts() const;

private:
  static constexpr const char *IMAGE_NAME = "singapore.jpg";
  static constexpr const char *FOLDER_NAME = "others";

  std::shared_ptr<PlatformInfo>               mPlatformInfo = nullptr;
  std::shared_ptr<DirectoryInspectionService> mDirectoryInspectionService =
      nullptr;
  std::shared_ptr<TaskCruncher> mTaskCruncher = nullptr;
  PBDev::ThreadScheduler       *mThreadScheduler = nullptr;

  PBDev::DirectoryInspectionJobId mLutsInspectionId =
      PBDev::DirectoryInspectionJobId(RuntimeUUID::newUUID());

  LutIconsPreprocessingJob mLutIconsPreprocessingJob;

  Path originalImagePath() const
  {
    return mPlatformInfo->installationPath / FOLDER_NAME / IMAGE_NAME;
  }

  Path lutAssetsPath() const;
};
} // namespace PB
