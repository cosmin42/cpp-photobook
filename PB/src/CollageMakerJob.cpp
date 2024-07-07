#include <pb/CollageMakerJob.h>

namespace PB {

CollageMakerJob::CollageMakerJob(Path localStatePath, Path installPath)
    : mCollagesResourcesPath(localStatePath), mInstallPath(installPath),
      mAssistant(installPath / COLLAGES_TEMPLATES_NAME, mCollagesResourcesPath),
      mLocalStatePath(localStatePath),
      mResources(std::make_shared<SkiaResources>()),
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

void CollageMakerJob::mapJobs(Path                           templatePath,
                              std::vector<std::vector<Path>> imagesPaths)
{
  PBDev::basicAssert(!mProjectId.empty());

  cv::Size imageSize = {mProject->paperSettings.width,
                        mProject->paperSettings.height};
  mResourcesProviderId =
      mResources->addResource(mLocalStatePath / "th" / mProjectId);

  for (auto i = 0; i < imagesPaths.size(); ++i) {
    auto reducerId = PBDev::MapReducerTaskId(RuntimeUUID::newUUID());
    mCollageIndex[reducerId] = (unsigned)i;
    mFunctions.push_back(
        {reducerId, [this, imagesPaths{imagesPaths.at(i)},
                     templatePath{templatePath}, imageSize{imageSize}]() {
           auto processedPath = mAssistant.createTemplateThumbnail(
               imagesPaths, templatePath, {4, 3}, imageSize);

           auto outFilePath =
               mLocalStatePath / "th" / mProjectId /
               (boost::uuids::to_string(boost::uuids::random_generator()()) +
                ".png");

           SkFILEWStream outFile(outFilePath.string().c_str());

           mDrawingService.renderToStream(mResourcesProviderId, outFile,
                                          processedPath, imageSize);
         }});
  }
}

std::optional<IdentifyableFunction>
CollageMakerJob::getNext(std::stop_token stopToken)
{
  if (mFunctions.size() > mIndex) {
    auto &f = mFunctions.at(mIndex);
    mIndex++;
    return f;
  }
  return std::nullopt;
}

void CollageMakerJob::onFinished(PBDev::MapReducerTaskId reducerTaskId)
{
  if (mCollageIndex.find(reducerTaskId) != mCollageIndex.end()) {
    mListener->onCollageCreated(mCollageIndex.at(reducerTaskId));
  }
}

} // namespace PB
