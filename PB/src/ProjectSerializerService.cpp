#include <pb/services/ProjectSerializerService.h>

#include <pb/components/Serializer.h>

#include <fstream>

namespace PB::Service {
void ProjectSerializerService::configurePlatformInfo(
    std::shared_ptr<PlatformInfo> platformInfo)
{
  mPlatformInfo = platformInfo;
}
Json ProjectSerializerService::loadAsJson(Path projectPath)
{
    std::ifstream stream(projectPath);
    if (!stream) {
        PBDev::basicAssert(false);
    }

    return Json::parse(stream);
}

void ProjectSerializerService::saveAsJson(Path projectPath, Json json)
{
  std::ofstream file(projectPath);

  PBDev::basicAssert(file.is_open());

  file << json.dump(2);
  file.close();
}

void ProjectSerializerService::createProjectFolderStructure(
    boost::uuids::uuid projectId)
{
  auto projectPath = mPlatformInfo->localStatePath / "projects" /
                     boost::uuids::to_string(projectId);

  std::filesystem::create_directory(mPlatformInfo->localStatePath / "projects");
  std::filesystem::create_directory(mPlatformInfo->localStatePath / "projects" /
                                    boost::uuids::to_string(projectId));
  std::filesystem::create_directories(
      mPlatformInfo->localStatePath / "projects" /
      boost::uuids::to_string(projectId) / "thumbnail-images");
}

Project ProjectSerializerService::deserializeProjectInfo(Path projectPath)
{
  auto jsonSerialization = loadAsJson(projectPath);

  Project project;
  from_json(jsonSerialization, project);

  return project;
}

std::vector<std::vector<GenericImagePtr>>
ProjectSerializerService::deserializeUnstagedImages(Path projectPath)
{
  auto jsonSerialization = loadAsJson(projectPath);

  std::vector<std::vector<GenericImagePtr>> unstagedImages;

  from_json(jsonSerialization.at("unstaged"), unstagedImages);

  return unstagedImages;
}

std::vector<GenericImagePtr>
ProjectSerializerService::deserializeStagedImages(Path projectPath)
{
  auto jsonSerialization = loadAsJson(projectPath);

  std::vector<GenericImagePtr> stagedImages;
  from_json(jsonSerialization.at("staged"), stagedImages);

  return stagedImages;
}

std::vector<Path> ProjectSerializerService::deserializeRoots(Path projectPath)
{
  auto jsonSerialization = loadAsJson(projectPath);

  std::vector<Path> roots;
  from_json(jsonSerialization.at("row-paths"), roots);

  return roots;
}

void ProjectSerializerService::saveProject(Project project)
{
  auto projectPath = mPlatformInfo->localStatePath / "projects" /
                     (project.name + OneConfig::BOOK_EXTENSION);

  Json json;
  to_json(json, project);

  // TODO: Separate this save to an infrastructure class, add DiskOperations
  // class
  saveAsJson(projectPath, json);
}
} // namespace PB::Service
