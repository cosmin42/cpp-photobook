#pragma once

#include <pb/DirectoryInspectionService.h>
#include <pb/LutIconsPreprocessingJob.h>
#include <pb/Platform.h>

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

  void detectLuts();

  void onInspectionFinished(PBDev::DirectoryInspectionJobId id,
                            std::vector<Path> searchResults) override;

  void onLutIconsPreprocessingFinished(Path icon) override {}

  std::unordered_set<PBDev::LutName, boost::hash<PBDev::LutName>>
  listLuts() const;

private:
  std::shared_ptr<PlatformInfo>               mPlatformInfo;
  std::shared_ptr<DirectoryInspectionService> mDirectoryInspectionService;
  std::shared_ptr<TaskCruncher>               mTaskCruncher;

  PBDev::DirectoryInspectionJobId mLutsInspectionId =
      PBDev::DirectoryInspectionJobId(RuntimeUUID::newUUID());

  LutIconsPreprocessingJob mLutIconsPreprocessingJob;

  Path lutAssetsPath() const;
};
} // namespace PB
