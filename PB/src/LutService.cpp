#include <pb/LutService.h>

#pragma warning(push)
#pragma warning(disable : 4996)
#include <spdlog/spdlog.h>
#pragma warning(pop)

namespace PB {

LutService::LutService()
{
  auto lutIconsPreprocessingListener =
      dynamic_cast<LutIconsPreprocessingListener *>(this);
  PBDev::basicAssert(lutIconsPreprocessingListener != nullptr);
  mLutIconsPreprocessingJob.configureListener(lutIconsPreprocessingListener);
}

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

void LutService::configureTaskCruncher(
    std::shared_ptr<TaskCruncher> taskCruncher)
{
  mTaskCruncher = taskCruncher;
}

void LutService::detectLuts()
{
  mLutsInspectionId = PBDev::DirectoryInspectionJobId(RuntimeUUID::newUUID());
  mDirectoryInspectionService->inspectDirectory(
      mLutsInspectionId, PBDev::ValidatorFunctionName("lut-validator"),
      lutAssetsPath());
}

void LutService::onInspectionFinished(PBDev::DirectoryInspectionJobId id,
                                      std::vector<Path> searchResults)
{
  mLutIconsPreprocessingJob.configureLuts(searchResults);
  mTaskCruncher->crunch("lut-icons", mLutIconsPreprocessingJob,
                        PBDev::ProgressJobName("lut-icons"));
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
