#include <pb/LutService.h>

#pragma warning(push)
#pragma warning(disable : 4996)
#include <spdlog/spdlog.h>
#pragma warning(pop)

namespace PB {

void LutService::configurePlatformInfo(
    std::shared_ptr<PlatformInfo> platformInfo)
{
  mPlatformInfo = platformInfo;
}

void LutService::configureDirectoryInspectionService(
    std::shared_ptr<DirectoryInspectionService> directoryInspectionService)
{
  mDirectoryInspectionService = directoryInspectionService;

  auto directoryInspectionJobListener =
      dynamic_cast<PB::DirectoryInspectionJobListener *>(this);
  PBDev::basicAssert(directoryInspectionJobListener != nullptr);
  mDirectoryInspectionService->configureListener(
      directoryInspectionJobListener);
}

void LutService::detectLuts()
{
  mLutsInspectionId = PBDev::DirectoryInspectionJobId(RuntimeUUID::newUUID());
  mDirectoryInspectionService->inspectDirectory(
      mLutsInspectionId, PBDev::ValidatorFunctionName("lut-validator"),
      lutAssetsPath());
}

void LutService::onFoundFile(PBDev::DirectoryInspectionJobId id, Path file) {}

void LutService::onInspectionFinished(PBDev::DirectoryInspectionJobId id,
                                      std::vector<Path> searchResults)
{
  for (auto &path : searchResults) {
    spdlog::info("Found LUT: {}", path.string());
  }
}

std::unordered_set<PBDev::LutName, boost::hash<PBDev::LutName>>
LutService::listLuts() const
{
  return std::unordered_set<PBDev::LutName, boost::hash<PBDev::LutName>>();
}

Path LutService::lutAssetsPath() const
{
  return mPlatformInfo->installationPath / "film-luts";
}
} // namespace PB
