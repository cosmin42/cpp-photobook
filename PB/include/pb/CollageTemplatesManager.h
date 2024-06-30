#pragma once

#include <vector>

#include <pb/CollageLibraryAssistant.h>
#include <pb/DrawingService.h>
#include <pb/SVGInflater.h>
#include <pb/project/Project.h>

namespace PB {
class CollageTemplatesManager {
public:
  explicit CollageTemplatesManager(Path localStatePath, Path installationPath,
                                   std::shared_ptr<Project> project);
  ~CollageTemplatesManager() = default;

  void generateTemplatesImages();

  std::vector<Path> getTemplatesPaths() const;

private:
  static constexpr const char *COLLAGES_TEMPLATES_RESOURCES_NAME =
      "collages-templates-resources";

  static constexpr const char *COLLAGES_TEMPLATES_NAME = "svg-templates";

  Path                           mCollagesTemplatesResourcesPath;
  Path                           mInstallPath;
  CollageLibraryAssistant        mAssistant;
  std::shared_ptr<SkiaResources> mResources = nullptr;
  DrawingService                 mDrawingService;
  std::shared_ptr<Project>       mProject = nullptr;
  std::vector<Path>              mGeneratedLibraries;

  PBDev::SkiaResourcesId mResourcesProviderId;

  std::vector<Path> mNumberedImages;

  std::vector<Path> getTemplatesPaths(Path directoryPath);
};
} // namespace PB
