#pragma once

#include <pb/DirectoryInspectionService.h>
#include <pb/Platform.h>

DECLARE_STRONG_STRING(LutName)

namespace PB {
class LutService final: public DirectoryInspectionJobListener {
public:
  ~LutService() = default;

  void configurePlatformInfo(std::shared_ptr<PlatformInfo> platformInfo);
  void configureDirectoryInspectionService(
      std::shared_ptr<DirectoryInspectionService> directoryInspectionService);

  void detectLuts();

  void onFoundFile(PBDev::DirectoryInspectionJobId id, Path file) override;
  void onInspectionFinished(PBDev::DirectoryInspectionJobId id,
                            std::vector<Path> searchResults) override;

  std::unordered_set<PBDev::LutName, boost::hash<PBDev::LutName>>
  listLuts() const;

private:
  std::shared_ptr<PlatformInfo>               mPlatformInfo;
  std::shared_ptr<DirectoryInspectionService> mDirectoryInspectionService;

  PBDev::DirectoryInspectionJobId mLutsInspectionId;

  Path lutAssetsPath() const;
};
} // namespace PB
