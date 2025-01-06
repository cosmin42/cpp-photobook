#include <pb/services/ProjectSerializerService.h>

#include <pb/components/SerializationStrategy.h>
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
  std::ifstream file(projectPath);

  PBDev::basicAssert(file.is_open());

  Json jsonData;
  try {
    file >> jsonData;
    file.close();
    return jsonData;
  }
  catch (Json::exception err) {
    auto what = err.what();
    UNUSED(what);
    PBDev::basicAssert(false);
  }
  return Json();
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

  auto projectDetailsOrError =
      PB::Text::deserialize<Project>(jsonSerialization);

  PBDev::basicAssert(std::holds_alternative<Project>(projectDetailsOrError));

  return std::get<Project>(projectDetailsOrError);
}

std::vector<std::vector<GenericImagePtr>>
ProjectSerializerService::deserializeUnstagedImages(Path projectPath)
{
  auto jsonSerialization = loadAsJson(projectPath);

  auto unstagedImagesOrError =
      PB::Text::deserialize<std::vector<std::vector<GenericImagePtr>>>(
          jsonSerialization.at("unstaged"));

  PBDev::basicAssert(
      std::holds_alternative<std::vector<std::vector<GenericImagePtr>>>(
          unstagedImagesOrError));

  return std::get<std::vector<std::vector<GenericImagePtr>>>(
      unstagedImagesOrError);
}

std::vector<GenericImagePtr>
ProjectSerializerService::deserializeStagedImages(Path projectPath)
{
  auto jsonSerialization = loadAsJson(projectPath);

  auto stagedImagesOrError =
      PB::Text::deserialize<std::vector<GenericImagePtr>>(
          jsonSerialization.at("staged"));

  PBDev::basicAssert(std::holds_alternative<std::vector<GenericImagePtr>>(
      stagedImagesOrError));

  return std::get<std::vector<GenericImagePtr>>(stagedImagesOrError);
}

std::vector<Path> ProjectSerializerService::deserializeRoots(Path projectPath)
{
  auto jsonSerialization = loadAsJson(projectPath);

  std::variant<std::vector<Path>, PBDev::Error> importedFoldersOrError;
#ifndef _CLANG_UML_
  importedFoldersOrError =
      PB::Text::deserializeSpecial(jsonSerialization, "row-paths");

  PBDev::basicAssert(
      std::holds_alternative<std::vector<Path>>(importedFoldersOrError));

  return std::get<std::vector<Path>>(importedFoldersOrError);
#endif
  return std::vector<Path>();
}

void ProjectSerializerService::saveProject(Project project)
{
  auto projectPath = mPlatformInfo->localStatePath / "projects" /
                     (project.name + OneConfig::BOOK_EXTENSION);

  auto jsonOrError = flatSimple<Project>(0, project);

  PBDev::basicAssert(std::holds_alternative<Json>(jsonOrError));

  // TODO: Separate tis save to an infrastructure class.
  saveAsJson(projectPath, std::get<Json>(jsonOrError));
}
} // namespace PB
