#include <pb/CollageMakerJob.h>

namespace PB {

CollageMakerJob::CollageMakerJob()
    : mResources(std::make_shared<SkiaResources>()),
      mDrawingService(mResources), mResourcesProviderId(RuntimeUUID::newUUID())
{
}

void CollageMakerJob::configureListener(CollageMakerListener *listener)
{
  mListener = listener;
}

void CollageMakerJob::configureProject(std::shared_ptr<Project> project)
{
  mProject = project;
}

void CollageMakerJob::configureProjectId(std::string projectId)
{
  mProjectId = projectId;
}

void CollageMakerJob::configurePlatformInfo(
    std::shared_ptr<PlatformInfo> platformInfo)
{
  mPlatformInfo = platformInfo;

  mAssistant =
      std::make_shared<CollageLibraryAssistant>(mPlatformInfo->localStatePath);
}

void CollageMakerJob::mapJobs(Path templatePath, std::vector<Path> imagesPaths)
{
  PBDev::basicAssert(!mProjectId.empty());

  cv::Size imageSize = {mProject->paperSettings.width,
                        mProject->paperSettings.height};

  auto resourcePath = mPlatformInfo->localStatePath / "th" / mProjectId / "";
  mResourcesProviderId = mResources->addResource(resourcePath);

  std::vector<Path> imagesNames;
  for (auto const &imagePath : imagesPaths) {
    imagesNames.push_back(imagePath.filename());
  }

  for (auto i = 0; i < imagesPaths.size() / 2; ++i) {

    std::string newImageName =
        boost::uuids::to_string(boost::uuids::random_generator()()) + ".png";

    Path projectThumbnailsRoot =
        mPlatformInfo->localStatePath / "th" / mProjectId;

    auto reducerId = PBDev::MapReducerTaskId(RuntimeUUID::newUUID());

    mCollageIndex[reducerId] = (unsigned)i;
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
  if (mCollageIndex.find(reducerTaskId) != mCollageIndex.end()) {
    mListener->onCollageCreated(mCollageIndex.at(reducerTaskId),
                                mCollagePath.at(reducerTaskId));
  }
}

} // namespace PB
