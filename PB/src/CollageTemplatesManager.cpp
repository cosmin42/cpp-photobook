#include <pb/CollageTemplatesManager.h>

#pragma warning(push)
#pragma warning(disable : 4996)
#include <spdlog/spdlog.h>
#pragma warning(pop)

namespace PB {
CollageTemplatesManager::CollageTemplatesManager(
    Path localStatePath, Path installPath, std::shared_ptr<Project> project)
    : mCollagesTemplatesResourcesPath(localStatePath /
                                      COLLAGES_TEMPLATES_RESOURCES_NAME / ""),
      mInstallPath(installPath),
      mAssistant(installPath / COLLAGES_TEMPLATES_NAME,
                 mCollagesTemplatesResourcesPath),
      mResources(std::make_shared<SkiaResources>()),
      mDrawingService(mResources), mProject(project),
      mResourcesProviderId(RuntimeUUID::newUUID())
{
}

std::vector<Path> CollageTemplatesManager::getTemplatesPaths(Path directoryPath)
{
  std::vector<Path> templatesPaths;

  for (const auto &entry : std::filesystem::directory_iterator(directoryPath)) {
    if (entry.is_regular_file()) {
      templatesPaths.push_back(entry.path());
    }
  }

  return templatesPaths;
}

void CollageTemplatesManager::generateTemplatesImages()
{
  auto templatesList =
      getTemplatesPaths(mInstallPath /
                                         COLLAGES_TEMPLATES_NAME);

  std::filesystem::create_directories(mCollagesTemplatesResourcesPath);

  std::vector<Path> processedSVGPaths;

  cv::Size imageSize = {mProject->paperSettings.width / 2,
                        mProject->paperSettings.height / 2};

  for (auto &path : templatesList) {
    spdlog::info("Generating thumbnail for template: {}", path.string());
    auto processedPath =
        mAssistant.createTemplateThumbnail(path, {4, 3}, imageSize);

    processedSVGPaths.push_back(processedPath);
  }

  mResourcesProviderId =
      mResources->addResource(mCollagesTemplatesResourcesPath);

  for (auto &path : processedSVGPaths) {
    Path outFilePath =
        mCollagesTemplatesResourcesPath / (path.stem().string() + ".png");
    SkFILEWStream outFile(outFilePath.string().c_str());

    mDrawingService.renderToStream(mResourcesProviderId, outFile, path,
                                   imageSize);
  }
}

} // namespace PB
