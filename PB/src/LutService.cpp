#include <pb/services/LutService.h>

#pragma warning(push)
#pragma warning(disable : 4996)
#include <spdlog/spdlog.h>
#pragma warning(pop)

#include <pb/components/Project.h>
#include <pb/components/ThumbnailsTask.h>

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

void LutService::configureProject(IdentifiableProject project)
{
  mProject = project;
}

void LutService::configureImageFactory(
    std::shared_ptr<ImageFactory> imageFactory)
{
  mImageFactory = imageFactory;
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
    mThreadScheduler->post([this, path{path}]() {
      mLutIconsPreprocessingJob.configureImagePath(path);
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

bool LutService::isRunning() const
{
  auto isDirectoryInspectionRunning =
      mDirectoryInspectionService->isRunning(mLutsInspectionId);
  auto isLutIconsPreprocessingRunning = !mLutIconsPreprocessingJob.isFinished();
  return isDirectoryInspectionRunning || isLutIconsPreprocessingRunning;
}

void LutService::stop()
{
  mDirectoryInspectionService->stop(mLutsInspectionId);
  mLutCreationStopSource.request_stop();
}

void LutService::onInspectionFinished(PBDev::DirectoryInspectionJobId id,
                                      std::vector<Path> searchResults)
{
  UNUSED(id);
  for (auto &path : searchResults) {
    if (lutExists(path)) {
      PBDev::basicAssert(
          mDurableHashService->maybeRetrieve(path.string()).has_value());
      Path iconPath = mDurableHashService->maybeRetrieve(path.string()).value();
      auto lutName = Job::LutIconsPreprocessingJob::extractNameFromPath(path);
      onLutIconsPreprocessingFinished(lutName, path, iconPath);
    }
    else {
      mLutsPathsToBeProcessed.push_back(path);
    }
    mLutsPaths.push_back(path);
  }

  mThreadScheduler->post([this, unprocessedPaths{mLutsPathsToBeProcessed}]() {
    mLutIconsPreprocessingJob.configureLuts(unprocessedPaths);
    mLutCreationStopSource =
        mTaskCruncher->crunch("lut-icons", mLutIconsPreprocessingJob,
                              PBDev::ProgressJobName("lut-icons"));
  });
}

void LutService::onLutIconsPreprocessingFinished(std::string lutName,
                                                 Path cubeFile, Path icon)
{
  LutIconInfo lutIconInfo;
  lutIconInfo.path = icon;
  lutIconInfo.name = lutName;
  mLutsIconsInfo.push_back(lutIconInfo);
  mDurableHashService->linkData(cubeFile.string(), icon.string());
  mLutServiceListener->onLutAdded(lutIconInfo);
}

void LutService::applyLut(PBDev::LutApplicationId lutId, unsigned lutIndex,
                          GenericImagePtr image)
{
  // generate a new uuid
  mOutImageHashes[lutId] = boost::uuids::to_string(RuntimeUUID::newUUID());

  LutImageProcessingData imageProcessingData;
  imageProcessingData.inImage =
      mPlatformInfo->thumbnailByHash(mProject->id, image->hash());
  imageProcessingData.outImage =
      mPlatformInfo->projectSupportFolder(mProject->id) /
      (mOutImageHashes.at(lutId) + OneConfig::JPG_EXTENSION);

  // TODO: Move this read to a different place. It shouldn't be here.
  auto lutData = infra::readLutData(mLutsPaths.at(lutIndex));

  for (auto const &data : lutData) {
    imageProcessingData.lut.push_back(
        cv::Vec4f(data[0], data[1], data[2], 1.0));
  }

  mTaskCruncher->crunch([this, imageProcessingData,
                         hash{mOutImageHashes[lutId]}, lutId]() {
    mOglEngine->applyLut(imageProcessingData);

    auto newHash = boost::uuids::to_string(boost::uuids::random_generator()());

    auto maybeNewHash = ThumbnailsTask::createThumbnailsByPath(
        imageProcessingData.outImage, mPlatformInfo, mProject, newHash);
    PBDev::basicAssert(maybeNewHash == newHash);
    auto newImage = mImageFactory->createRegularImage(newHash);

    std::filesystem::remove(imageProcessingData.outImage);

    spdlog::info("LUT applied {}", newImage->full().string());

    mLutServiceListener->onLutApplied(lutId, newImage);
  });
}

void LutService::applyLutInMemory(PBDev::LutApplicationId lutId,
                                  unsigned lutIndex, cv::Mat image)
{
  LutInMemoryData imageProcessingData;
  imageProcessingData.inImage = image;
  imageProcessingData.outImage = cv::Mat::zeros(image.size(), image.type());

  auto lutData = infra::readLutData(mLutsPaths.at(lutIndex));

  for (auto const &data : lutData) {
    imageProcessingData.lut.push_back(
        cv::Vec4f(data[0], data[1], data[2], 1.0));
  }

  mTaskCruncher->crunch([this, imageProcessingData, lutId]() {
    mOglEngine->applyLutInMemory(imageProcessingData);
    auto imagePtr = std::make_shared<cv::Mat>(imageProcessingData.outImage);
    mLutServiceListener->onLutAppliedInMemory(lutId, imagePtr);
  });
}

std::vector<LutIconInfo> LutService::listLuts() const { return mLutsIconsInfo; }

Path LutService::lutAssetsPath() const
{
  return mPlatformInfo->installationPath / "film-luts";
}

bool LutService::lutExists(const Path &path) const
{
  if (!mDurableHashService->maybeRetrieve(path.string()).has_value()) {
    return false;
  }

  auto correspondingPath = Path(mDurableHashService->maybeRetrieve(path.string()).value());

  if (!std::filesystem::exists(correspondingPath)) {
    return false;
  }

  return infra::isValidImage(correspondingPath);
}

void LutService::applyTransformationOnDisk(PBDev::LutApplicationId lutId,
                                           unsigned                lutIndex,
                                           GenericImagePtr image, bool inplace)
{
  auto newHash = boost::uuids::to_string(boost::uuids::random_generator()());

  LutImageProcessingData largeProcessingData;
  LutImageProcessingData mediumProcessingData;
  LutImageProcessingData smallProcessingData;

  auto lutData = infra::readLutData(mLutsPaths.at(lutIndex));

  for (auto const &data : lutData) {
    largeProcessingData.lut.push_back(
        cv::Vec4f(data[0], data[1], data[2], 1.0));
    mediumProcessingData.lut.push_back(
        cv::Vec4f(data[0], data[1], data[2], 1.0));
    smallProcessingData.lut.push_back(
        cv::Vec4f(data[0], data[1], data[2], 1.0));
  }

  largeProcessingData.inImage = mPlatformInfo->thumbnailByHash(
      mProject->id, image->hash(), ThumbnailsSize::LARGE);
  mediumProcessingData.inImage = mPlatformInfo->thumbnailByHash(
      mProject->id, image->hash(), ThumbnailsSize::MEDIUM);
  smallProcessingData.inImage = mPlatformInfo->thumbnailByHash(
      mProject->id, image->hash(), ThumbnailsSize::SMALL);

  if (inplace) {
    largeProcessingData.outImage = largeProcessingData.inImage;
    mediumProcessingData.outImage = mediumProcessingData.inImage;
    smallProcessingData.outImage = smallProcessingData.inImage;
  }
  else {
    largeProcessingData.outImage = mPlatformInfo->thumbnailByHash(
        mProject->id, newHash, ThumbnailsSize::LARGE);
    mediumProcessingData.outImage = mPlatformInfo->thumbnailByHash(
        mProject->id, newHash, ThumbnailsSize::MEDIUM);
    smallProcessingData.outImage = mPlatformInfo->thumbnailByHash(
        mProject->id, newHash, ThumbnailsSize::SMALL);
  }

  mTaskCruncher->crunch([this, largeProcessingData]() {
    mOglEngine->applyLut(largeProcessingData);
    Noir::inst().getLogger()->info("LUT applied {}",
                                   largeProcessingData.outImage.string());
  });

  mTaskCruncher->crunch([this, mediumProcessingData]() {
    mOglEngine->applyLut(mediumProcessingData);
  });

  mTaskCruncher->crunch([this, smallProcessingData, lutId, inplace, newHash]() {
    mOglEngine->applyLut(smallProcessingData);
    if (inplace) {
      mLutServiceListener->onLutAppliedOnDiskInplace(lutId);
    }
    else {
      GenericImagePtr newImage = std::make_shared<RegularImageV2>(newHash, "");
      mLutServiceListener->onLutAppliedOnDisk(lutId, newImage);
    }
  });
}

void LutService::applyLutAndEffects(PBDev::LutApplicationId lutId,
                                    unsigned lutIndex, GenericImagePtr image,
                                    double saturation, double contrast,
                                    double brightness)
{
  auto lutData = infra::readLutData(mLutsPaths.at(lutIndex));

  LutImageProcessingAndEffectsData largeProcessingData;
  LutImageProcessingAndEffectsData mediumProcessingData;
  LutImageProcessingAndEffectsData smallProcessingData;

  for (auto const &data : lutData) {
    largeProcessingData.lut.push_back(
        cv::Vec4f(data[0], data[1], data[2], 1.0));
    mediumProcessingData.lut.push_back(
        cv::Vec4f(data[0], data[1], data[2], 1.0));
    smallProcessingData.lut.push_back(
        cv::Vec4f(data[0], data[1], data[2], 1.0));
  }

  largeProcessingData.inImage = largeProcessingData.outImage = image->full();
  largeProcessingData.saturation = saturation;
  largeProcessingData.contrast = contrast;
  largeProcessingData.brightness = brightness;

  mediumProcessingData.inImage = mediumProcessingData.outImage =
      image->medium();
  mediumProcessingData.saturation = saturation;
  mediumProcessingData.contrast = contrast;
  mediumProcessingData.brightness = brightness;

  smallProcessingData.inImage = smallProcessingData.outImage = image->smaLL();
  smallProcessingData.saturation = saturation;
  smallProcessingData.contrast = contrast;
  smallProcessingData.brightness = brightness;

  mTaskCruncher->crunch([this, largeProcessingData, lutId]() {
    mOglEngine->applyLutAndEffects(largeProcessingData);
    mLutServiceListener->onLutAppliedOnDiskInplace(lutId);
  });
}

} // namespace PB::Service
