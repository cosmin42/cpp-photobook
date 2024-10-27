#include <pb/services/LutService.h>

#pragma warning(push)
#pragma warning(disable : 4996)
#include <spdlog/spdlog.h>
#pragma warning(pop)

namespace PB::Service {

LutService::LutService()
    : mDirectoryInspectionService(
          std::make_shared<DirectoryInspectionService>())
{
  auto lutIconsPreprocessingListener =
      dynamic_cast<LutIconsPreprocessingListener *>(this);
  PBDev::basicAssert(lutIconsPreprocessingListener != nullptr);
  mLutIconsPreprocessingJob.configureListener(lutIconsPreprocessingListener);

  auto directoryInspectionJobListener =
      dynamic_cast<DirectoryInspectionJobListener *>(this);
  PBDev::basicAssert(directoryInspectionJobListener != nullptr);
  mDirectoryInspectionService->configureListener(
      directoryInspectionJobListener);
}

void LutService::configurePlatformInfo(
    std::shared_ptr<PlatformInfo> platformInfo)
{
  mPlatformInfo = platformInfo;
  mLutIconsPreprocessingJob.configurePlatformInfo(platformInfo);
}

void LutService::configureTaskCruncher(
    std::shared_ptr<TaskCruncher> taskCruncher)
{
  mTaskCruncher = taskCruncher;
  mDirectoryInspectionService->configureTaskCruncher(taskCruncher);
}

void LutService::configureThreadScheduler(
    PBDev::ThreadScheduler *threadScheduler)
{
  mThreadScheduler = threadScheduler;
  mDirectoryInspectionService->configureThreadScheduler(threadScheduler);
}

void LutService::configureLutServiceListener(LutServiceListener *listener)
{
  mLutServiceListener = listener;
}

void LutService::startLutService()
{
  if (!std::filesystem::exists(mPlatformInfo->processedLutsPath())) {
    std::filesystem::create_directory(mPlatformInfo->processedLutsPath());
  }
  mTaskCruncher->crunch([this]() {
    auto path = originalImagePath();
    auto originalImage = ImageReader().loadImage(path);
    mThreadScheduler->post([this, originalImage]() {
      mLutIconsPreprocessingJob.configureOriginalImage(originalImage);
      detectLuts();
    });
  });
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
  UNUSED(id);

  mThreadScheduler->post([this, searchResults{searchResults}]() {
    mLutIconsPreprocessingJob.configureLuts(searchResults);
    mTaskCruncher->crunch("lut-icons", mLutIconsPreprocessingJob,
                          PBDev::ProgressJobName("lut-icons"));
  });
}

void LutService::onLutIconsPreprocessingFinished(std::string lutName, Path icon)
{
  LutIconInfo lutIconInfo;
  lutIconInfo.path = icon;
  lutIconInfo.name = lutName;
  mLutsPaths.push_back(lutIconInfo);
  mLutServiceListener->onLutAdded(lutIconInfo);
}

std::vector<LutIconInfo> LutService::listLuts() const { return mLutsPaths; }

Path LutService::lutAssetsPath() const
{
  return mPlatformInfo->installationPath / "film-luts";
}

bool LutService::lutExists(const Path &path) const
{
  if (!std::filesystem::exists(path)) {
    return false;
  }

  return ImageReader().isValid(path);
}

} // namespace PB::Service
