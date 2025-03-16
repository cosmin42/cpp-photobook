#include <pb/jobs/CollageThumbnailsMakerJob.h>

#pragma warning(push)
#pragma warning(disable : 4996)
#include <spdlog/spdlog.h>
#pragma warning(pop)

#include <regex>

namespace PB::Job {

CollageThumbnailsMakerJob::CollageThumbnailsMakerJob()
    : mResources(std::make_shared<SkiaResources>()),
      mDrawingService(mResources), mResourcesProviderId(RuntimeUUID::newUUID())
{
}

void CollageThumbnailsMakerJob::configureListener(
    CollageThumbnailsMakerListener *listener)
{
  mListener = listener;
}

void CollageThumbnailsMakerJob::configureProject(IdentifiableProject project)
{
  mProject = project;
  mAssistant = std::make_shared<CollageLibraryAssistant>(
      mPlatformInfo->collagesFolder(mProject->id));
}

void CollageThumbnailsMakerJob::configurePlatformInfo(
    std::shared_ptr<PlatformInfo> platformInfo)
{
  mPlatformInfo = platformInfo;
}

void CollageThumbnailsMakerJob::configureVulkanManager(
    std::shared_ptr<VulkanManager> vulkanManager)
{
  mDrawingService.configureVulkanManager(vulkanManager);
}

std::vector<CollageTemplateInfo>
CollageThumbnailsMakerJob::getTemplatesPaths(Path directoryPath)
{
  std::vector<CollageTemplateInfo> collageTemplatesInfo;

  for (const auto &entry : std::filesystem::directory_iterator(directoryPath)) {
    if (entry.is_regular_file()) {
      auto collageTemplateInfo = parseTemplatePath(entry.path());
      collageTemplatesInfo.push_back(collageTemplateInfo);
    }
  }

  return collageTemplatesInfo;
}

CollageTemplateInfo CollageThumbnailsMakerJob::parseTemplatePath(Path path)
{
  CollageTemplateInfo collageTemplateInfo;

  collageTemplateInfo.path = path;
  collageTemplateInfo.name = path.stem().string();
  collageTemplateInfo.name =
      std::regex_replace(collageTemplateInfo.name, std::regex("-"), " ");
  collageTemplateInfo.name =
      std::regex_replace(collageTemplateInfo.name, std::regex("_"), " ");

  std::string imageCountStr =
      collageTemplateInfo.name.substr(0, collageTemplateInfo.name.find('-'));

  try {
    collageTemplateInfo.imageCount = std::stoi(imageCountStr);
  }
  catch (std::invalid_argument &) {
    PBDev::basicAssert(false);
  }

  if (collageTemplateInfo.path.extension().string() != ".template") {
    std::istringstream       iss(collageTemplateInfo.name);
    std::vector<std::string> words{std::istream_iterator<std::string>{iss},
                                   std::istream_iterator<std::string>{}};
    collageTemplateInfo.name = "";

    PBDev::basicAssert(words.size() > 3);

    for (auto i = 1; i < words.size() - 2; ++i) {
      std::string word = words[i];
      PBDev::basicAssert(word.size() > 0);
      word[0] = (char)std::toupper(word[0]);
      collageTemplateInfo.name += word + " ";
    }
  }

  return collageTemplateInfo;
}

std::vector<CollageTemplateInfo>
CollageThumbnailsMakerJob::getTemplatesPaths() const
{
  return mGeneratedLibraries;
}

std::vector<CollageTemplateInfo>
CollageThumbnailsMakerJob::getSourceTemplates() const
{
  return mSourceTemplates;
}

void CollageThumbnailsMakerJob::createPlaceholdersFolder()
{
  auto collagesPath = mPlatformInfo->collagesFolder(mProject->id);
  bool success = std::filesystem::create_directories(collagesPath);
  if (success) {
    spdlog::info("[CollageThumbnailsMakerJob] Created placeholders folder for "
                 "project: {}",
                 boost::uuids::to_string(mProject->id));
  }
}

void CollageThumbnailsMakerJob::obtainSourceTemplates()
{
  mSourceTemplates = getTemplatesPaths(mPlatformInfo->installationPath /
                                       COLLAGES_TEMPLATES_NAME);
}

void CollageThumbnailsMakerJob::createNumberedPlaceholders()
{
  cv::Size imageSize = {mProject->value.paperSettings.width / 2,
                        mProject->value.paperSettings.height / 2};

  mNumberedImages = mAssistant->createNumberedImages(imageSize);
}

void CollageThumbnailsMakerJob::createCustomSVGTemplate(unsigned i)
{
  cv::Size imageSize = {mProject->value.paperSettings.width / 2,
                        mProject->value.paperSettings.height / 2};

  Path path = mSourceTemplates.at(i).path;
#ifndef _CLANG_UML_
  spdlog::info("Generating thumbnail for template: {}", path.string());
#endif
  auto processedPath = mAssistant->createTemplateThumbnail(
      mNumberedImages, path, {4, 3}, imageSize);

  mProcessedSVGPaths.push_back(processedPath);
}

void CollageThumbnailsMakerJob::registerNewResource()
{
  mResourcesProviderId =
      mResources->addResource(mPlatformInfo->collagesFolder(mProject->id));
}

void CollageThumbnailsMakerJob::createTemplatesThumbnail(unsigned i)
{
  cv::Size imageSize = {mProject->value.paperSettings.width / 2,
                        mProject->value.paperSettings.height / 2};
  Path     path = mProcessedSVGPaths.at(i);
  Path     outFilePath = mPlatformInfo->collagesFolder(mProject->id) /
                     (path.stem().string() + ".png");
  if (std::filesystem::exists(outFilePath)) {
    auto collageTemplateInfo = parseTemplatePath(outFilePath);
    mGeneratedLibraries.push_back(collageTemplateInfo);
    return;
  }
  SkFILEWStream outFile(outFilePath.string().c_str());

  mDrawingService.renderToStream(mResourcesProviderId, outFile, path,
                                 imageSize);
  auto collageTemplateInfo = parseTemplatePath(outFilePath);
  mGeneratedLibraries.push_back(collageTemplateInfo);
}

void CollageThumbnailsMakerJob::mapJobs()
{
  auto templatesList = getTemplatesPaths(mPlatformInfo->installationPath /
                                         COLLAGES_TEMPLATES_NAME);

  std::filesystem::create_directories(
      mPlatformInfo->collagesFolder(mProject->id));

  std::vector<Path> processedSVGPaths;

  cv::Size imageSize = {mProject->value.paperSettings.width / 2,
                        mProject->value.paperSettings.height / 2};

  mNumberedImages = mAssistant->createNumberedImages(imageSize);

  for (auto &collageTemplateInfo : templatesList) {
#ifndef _CLANG_UML_
    spdlog::info("Generating thumbnail for template: {}",
                 collageTemplateInfo.path.string());
#endif
    auto processedPath = mAssistant->createTemplateThumbnail(
        mNumberedImages, collageTemplateInfo.path, {4, 3}, imageSize);

    processedSVGPaths.push_back(processedPath);
  }

  mResourcesProviderId =
      mResources->addResource(mPlatformInfo->collagesFolder(mProject->id));

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
CollageThumbnailsMakerJob::getTask(std::stop_token stopToken)
{
  if (mFunctions.size() > mIndex) {
    auto &f = mFunctions.at(mIndex);
    mIndex++;
    return f;
  }
  return std::nullopt;
}

void CollageThumbnailsMakerJob::onTaskFinished(PBDev::MapReducerTaskId)
{
  mListener->onThumbnailsCreated();
}

} // namespace PB::Job
