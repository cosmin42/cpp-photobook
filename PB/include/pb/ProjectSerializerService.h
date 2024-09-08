#pragma once

#include <pb/Platform.h>
#include <pb/image/VirtualImage.h>
#include <pb/project/Project.h>
#include <pb/util/Traits.h>

namespace PB {
class ProjectSerializerService final {
public:
  ~ProjectSerializerService() = default;

  void configurePlatformInfo(std::shared_ptr<PlatformInfo> platformInfo);

  void createProjectFolderStructure(boost::uuids::uuid projectId);

  Project deserializeProjectInfo(Path projectPath);

  std::vector<std::vector<std::shared_ptr<VirtualImage>>>
  deserializeUnstagedImages(Path projectPath);

  std::vector<std::shared_ptr<VirtualImage>>
  deserializeStagedImages(Path projectPath);

  std::vector<Path> deserializeRoots(Path projectPath);

  void saveProject(
      std::string projectName, Project project,
      std::vector<std::vector<std::shared_ptr<VirtualImage>>> const
                                                       &unstagedImages,
      std::vector<std::shared_ptr<VirtualImage>> const &stagedImages,
      std::vector<Path> const                          &roots);

private:
  std::shared_ptr<PlatformInfo> mPlatformInfo = nullptr;

  Json loadAsJson(Path projectPath);

  void saveAsJson(Path projectPath, Json json);
};
} // namespace PB
