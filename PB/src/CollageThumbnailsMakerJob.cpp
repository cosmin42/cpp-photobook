#include <pb/CollageThumbnailsMakerJob.h>

#pragma warning(push)
#pragma warning(disable : 4996)
#include <spdlog/spdlog.h>
#pragma warning(pop)

namespace PB {

CollageThumbnailsMakerJob::CollageThumbnailsMakerJob(Path localStatePath,
                                                     Path installPath)
    : mCollagesTemplatesResourcesPath(localStatePath /
                                      COLLAGES_TEMPLATES_RESOURCES_NAME / ""),
      mInstallPath(installPath),
      mAssistant(installPath / COLLAGES_TEMPLATES_NAME,
                 mCollagesTemplatesResourcesPath),
      mResources(std::make_shared<SkiaResources>()),
      mDrawingService(mResources), mResourcesProviderId(RuntimeUUID::newUUID())
{
}

void CollageThumbnailsMakerJob::configureListener(
    CollageThumbnailsMakerListener *listener)
{
  mListener = listener;
}

void CollageThumbnailsMakerJob::configureProject(
    std::shared_ptr<Project> project)
{
  mProject = project;
}

std::vector<Path>
CollageThumbnailsMakerJob::getTemplatesPaths(Path directoryPath)
{
  std::vector<Path> templatesPaths;

  for (const auto &entry : std::filesystem::directory_iterator(directoryPath)) {
    if (entry.is_regular_file()) {
      templatesPaths.push_back(entry.path());
    }
  }

  return templatesPaths;
}

std::vector<Path> CollageThumbnailsMakerJob::getTemplatesPaths() const
{
  return mGeneratedLibraries;
}

std::vector<Path> CollageThumbnailsMakerJob::getSourceTemplates() const
{
  return mSourceTemplates;
}

void CollageThumbnailsMakerJob::createPlaceholdersFolder()
{
  std::filesystem::create_directories(mCollagesTemplatesResourcesPath);
}

void CollageThumbnailsMakerJob::obtainSourceTemplates()
{
  mSourceTemplates = getTemplatesPaths(mInstallPath / COLLAGES_TEMPLATES_NAME);
}

void CollageThumbnailsMakerJob::createNumberedPlaceholders()
{
  cv::Size imageSize = {mProject->paperSettings.width / 2,
                        mProject->paperSettings.height / 2};

  mNumberedImages = mAssistant.createNumberedImages(imageSize);
}

void CollageThumbnailsMakerJob::createCustomSVGTemplate(unsigned i)
{
  cv::Size imageSize = {mProject->paperSettings.width / 2,
                        mProject->paperSettings.height / 2};

  Path path = mSourceTemplates.at(i);
  spdlog::info("Generating thumbnail for template: {}", path.string());
  auto processedPath = mAssistant.createTemplateThumbnail(mNumberedImages, path,
                                                          {4, 3}, imageSize);

  mProcessedSVGPaths.push_back(processedPath);
}

void CollageThumbnailsMakerJob::registerNewResource()
{
  mResourcesProviderId =
      mResources->addResource(mCollagesTemplatesResourcesPath);
}

void CollageThumbnailsMakerJob::createTemplatesThumbnail(unsigned i)
{
  cv::Size imageSize = {mProject->paperSettings.width / 2,
                        mProject->paperSettings.height / 2};
  Path     path = mProcessedSVGPaths.at(i);
  Path     outFilePath =
      mCollagesTemplatesResourcesPath / (path.stem().string() + ".png");
  SkFILEWStream outFile(outFilePath.string().c_str());

  mDrawingService.renderToStream(mResourcesProviderId, outFile, path,
                                 imageSize);

  mGeneratedLibraries.push_back(outFilePath);
}

void CollageThumbnailsMakerJob::mapJobs()
{
  auto templatesList =
      getTemplatesPaths(mInstallPath / COLLAGES_TEMPLATES_NAME);

  std::filesystem::create_directories(mCollagesTemplatesResourcesPath);

  std::vector<Path> processedSVGPaths;

  cv::Size imageSize = {mProject->paperSettings.width / 2,
                        mProject->paperSettings.height / 2};

  PBDev::basicAssert(mNumberedImages.empty());

  mNumberedImages = mAssistant.createNumberedImages(imageSize);

  for (auto &path : templatesList) {
    spdlog::info("Generating thumbnail for template: {}", path.string());
    auto processedPath = mAssistant.createTemplateThumbnail(
        mNumberedImages, path, {4, 3}, imageSize);

    processedSVGPaths.push_back(processedPath);
  }

  mResourcesProviderId =
      mResources->addResource(mCollagesTemplatesResourcesPath);

  mFunctions.push_back(
      {PBDev::MapReducerTaskId(RuntimeUUID::newUUID()), [this]() {
         createPlaceholdersFolder();
         obtainSourceTemplates();
         createNumberedPlaceholders();
         for (int i = 0; i < mSourceTemplates.size(); ++i) {
           createCustomSVGTemplate(i);
         }
         registerNewResource();
         for (int i = 0; i < mProcessedSVGPaths.size(); ++i) {
           createTemplatesThumbnail(i);
         }
       }});
}

std::optional<IdentifyableFunction>
CollageThumbnailsMakerJob::getNext(std::stop_token stopToken)
{
  if (mFunctions.size() > mIndex) {
    auto &f = mFunctions.at(mIndex);
    mIndex++;
    return f;
  }
  return std::nullopt;
}

void CollageThumbnailsMakerJob::onFinished(PBDev::MapReducerTaskId)
{
  mListener->onThumbnailsCreated();
}
} // namespace PB
