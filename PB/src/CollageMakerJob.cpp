#include <pb/jobs/CollageMakerJob.h>

#include <pb/components/ImportImageTask.h>
#include <pb/components/ThumbnailsTask.h>
#include <pb/entities/CollageImage.h>

namespace PB::Job {

CollageMakerJob::CollageMakerJob()
    : mResources(std::make_shared<SkiaResources>()),
      mDrawingService(mResources), mResourcesProviderId(RuntimeUUID::newUUID())
{
}

void CollageMakerJob::configureListener(CollageMakerListener *listener)
{
  mListener = listener;
}

void CollageMakerJob::configureProject(IdentifiableProject project)
{
  mProject = project;
}

void CollageMakerJob::configureImageFactory(
    std::shared_ptr<ImageFactory> imageFactory)
{
  mImageFactory = imageFactory;
}

void CollageMakerJob::configurePlatformInfo(
    std::shared_ptr<PlatformInfo> platformInfo)
{
  mPlatformInfo = platformInfo;

  mAssistant =
      std::make_shared<CollageLibraryAssistant>(mPlatformInfo->localStatePath);
}

void CollageMakerJob::configureVulkanManager(
    std::shared_ptr<VulkanManager> vulkanManager)
{
  mDrawingService.configureVulkanManager(vulkanManager);
}

void CollageMakerJob::mapJobs(Path templatePath, std::vector<Path> imagesPaths, std::string accumulatedName)
{
  PBDev::basicAssert(mProject != nullptr);

  for(auto const &imagePath : imagesPaths) {
    mSources.push_back(imagePath);
  }
  
  mNewImageName = accumulatedName;

  cv::Size imageSize = {mProject->value.paperSettings.width,
                        mProject->value.paperSettings.height};

  auto resourcePath = mPlatformInfo->projectSupportFolder(mProject->id);

  mResourcesProviderId =
      mResources->addResource(resourcePath / "thumbnail-images");

  std::vector<Path> imagesNames;
  for (auto const &imagePath : imagesPaths) {
    imagesNames.push_back(Path("thumbnail-images") / imagePath.filename());
  }

  std::string newImageName =
      boost::uuids::to_string(boost::uuids::random_generator()()) + ".jpeg";

  Path projectThumbnailsRoot =
      mPlatformInfo->projectSupportFolder(mProject->id);

  auto reducerId = PBDev::MapReducerTaskId(RuntimeUUID::newUUID());

  mCollagePath[reducerId] = projectThumbnailsRoot / newImageName;

  mFunctions.push_back(
      {reducerId,
       [this, imagesNames{imagesNames}, templatePath{templatePath},
        imageSize{imageSize}, projectThumbnailsRoot{projectThumbnailsRoot},
        newImageName{newImageName}]() {
         Path temporarySvgFilePath =
             projectThumbnailsRoot / TEMPORARY_SVG_FILE_NAME;

         auto processedPath = mAssistant->createTemplateThumbnail(
             imagesNames, templatePath, {4, 3}, imageSize,
             temporarySvgFilePath.string());

         auto outFilePath = projectThumbnailsRoot / newImageName;

         SkFILEWStream outFile(outFilePath.string().c_str());

         mDrawingService.renderToStream(mResourcesProviderId, outFile,
                                        processedPath, imageSize);

         std::filesystem::remove(temporarySvgFilePath);
       }});
}

std::optional<IdentifyableFunction>
CollageMakerJob::getTask(std::stop_token stopToken)
{
  if (mFunctions.size() > mIndex) {
    auto &f = mFunctions.at(mIndex);
    mIndex++;
    return f;
  }
  return std::nullopt;
}

void CollageMakerJob::onTaskFinished(PBDev::MapReducerTaskId reducerTaskId)
{
  PBDev::basicAssert(mCollagePath.find(reducerTaskId) != mCollagePath.end());
  PBDev::basicAssert(mProject != nullptr);

  auto newHash = boost::uuids::to_string(boost::uuids::random_generator()());

  auto maybeNewHash = ThumbnailsTask::createThumbnailsByPath(
      mCollagePath.at(reducerTaskId), mPlatformInfo, mProject, newHash);

  PBDev::basicAssert(maybeNewHash == newHash);
  auto newImage = std::make_shared<CollageImage>(newHash, mSources, mNewImageName);

  std::filesystem::remove(mCollagePath.at(reducerTaskId));

  auto newImagePath =
      mPlatformInfo->thumbnailByHash(mProject->id, newImage->hash());

  spdlog::info("Collage created {}", newImagePath.string());

  mListener->onCollageCreated(newImage);
}

} // namespace PB::Job
