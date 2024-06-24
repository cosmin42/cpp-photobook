#include <pb/CollageTemplatesManager.h>

#pragma warning(push)
#pragma warning(disable : 4996)
#include <spdlog/spdlog.h>
#pragma warning(pop)

namespace PB {
CollageTemplatesManager::CollageTemplatesManager(
    Path installPath, std::shared_ptr<Project> project)
    : mCollagesTemplatesResourcesPath(installPath /
                                      COLLAGES_TEMPLATES_RESOURCES_NAME / ""),
      mAssistant(mCollagesTemplatesResourcesPath), mDrawingService(mResources),
      mProject(project), mResourcesProviderId(RuntimeUUID::newUUID())
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
  auto templatesList = getTemplatesPaths(std::filesystem::current_path() /
                                         COLLAGES_TEMPLATES_NAME);

  for (auto &path : templatesList) {
    spdlog::info("Generating thumbnail for template: {}", path.filename().string());
    mAssistant.createTemplateThumbnail(path.filename(), {4, 3},
                                       {mProject->paperSettings.width / 2,
                                        mProject->paperSettings.height / 2});
  }

  mResourcesProviderId =
      mResources->addResource(mCollagesTemplatesResourcesPath);
}

} // namespace PB
