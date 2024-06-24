#pragma once

#include <pb/CollageLibraryAssistant.h>
#include <pb/DrawingService.h>
#include <pb/SVGInflater.h>
#include <pb/project/Project.h>

namespace PB {
class CollageTemplatesManager {
public:
  explicit CollageTemplatesManager(Path                     installPath,
                                   std::shared_ptr<Project> project);
  ~CollageTemplatesManager() = default;

  void generateTemplatesImages();

private:
  Path                           mCollagesTemplatesResourcesPath;
  CollageLibraryAssistant        mAssistant;
  std::shared_ptr<SkiaResources> mResources = nullptr;
  DrawingService                 mDrawingService;
  std::shared_ptr<Project>       mProject = nullptr;

  // TODO: Initialize with a default value.
  PBDev::SkiaResourcesId mResourcesProviderId;

  static constexpr const char *COLLAGES_TEMPLATES_RESOURCES_NAME =
      "collages-templates-resources";

  static constexpr const char *COLLAGES_TEMPLATES_NAME = "svg-templates";

  std::vector<Path> getTemplatesPaths(Path directoryPath);
};
} // namespace PB
