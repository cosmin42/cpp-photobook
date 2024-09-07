#include <pb/ProjectSerializerService.h>

#include <pb/persistence/SerializationStrategy.h>

#include <fstream>

namespace PB {
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

  std::filesystem::create_directory(projectPath);
  std::filesystem::create_directory(projectPath / "thumbnail-images");
}

Project ProjectSerializerService::deserializeProjectInfo(Path projectPath)
{
  auto jsonSerialization = loadAsJson(projectPath);

  auto projectDetailsOrError =
      PB::Text::deserialize<Project>(jsonSerialization);

  PBDev::basicAssert(std::holds_alternative<Project>(projectDetailsOrError));

  return std::get<Project>(projectDetailsOrError);
}

std::vector<std::vector<std::shared_ptr<VirtualImage>>>
ProjectSerializerService::deserializeUnstagedImages(Path projectPath)
{
  auto jsonSerialization = loadAsJson(projectPath);

  auto unstagedImagesOrError = PB::Text::deserialize<
      std::vector<std::vector<std::shared_ptr<VirtualImage>>>>(
      jsonSerialization.at("unstaged"));

  PBDev::basicAssert(std::holds_alternative<
                     std::vector<std::vector<std::shared_ptr<VirtualImage>>>>(
      unstagedImagesOrError));

  return std::get<std::vector<std::vector<std::shared_ptr<VirtualImage>>>>(
      unstagedImagesOrError);
}

std::vector<std::shared_ptr<VirtualImage>>
ProjectSerializerService::deserializeStagedImages(Path projectPath)
{
  auto jsonSerialization = loadAsJson(projectPath);

  auto stagedImagesOrError =
      PB::Text::deserialize<std::vector<std::shared_ptr<VirtualImage>>>(
          jsonSerialization.at("staged"));

  PBDev::basicAssert(
      std::holds_alternative<std::vector<std::shared_ptr<VirtualImage>>>(
          stagedImagesOrError));

  return std::get<std::vector<std::shared_ptr<VirtualImage>>>(
      stagedImagesOrError);
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

void ProjectSerializerService::serializeProject(
    std::string projectName, Project project,
    std::vector<std::vector<std::shared_ptr<VirtualImage>>> const
                                                     &unstagedImages,
    std::vector<std::shared_ptr<VirtualImage>> const &stagedImages,
    std::vector<Path> const                          &roots)
{
  auto projectPath = mPlatformInfo->localStatePath / "projects" /
                     (projectName + OneConfig::BOOK_EXTENSION);

  auto jsonOrError = PB::Text::serialize<PB::Project>(0, {"root", project});

  PBDev::basicAssert(std::holds_alternative<Json>(jsonOrError));

  auto imageJsonOrError = PB::Text::serialize<
      std::vector<std::vector<std::shared_ptr<VirtualImage>>>>(
      0, {"unstaged", unstagedImages});

  PBDev::basicAssert(std::holds_alternative<Json>(imageJsonOrError));

  std::get<Json>(jsonOrError).update(std::get<Json>(imageJsonOrError));

  imageJsonOrError =
      PB::Text::serialize<std::vector<std::shared_ptr<VirtualImage>>>(
          0, {"staged", stagedImages});

  PBDev::basicAssert(std::holds_alternative<Json>(imageJsonOrError));

  std::get<Json>(jsonOrError).update(std::get<Json>(imageJsonOrError));

  imageJsonOrError =
      PB::Text::serialize<std::vector<Path>>(0, {"row-paths", roots});

  PBDev::basicAssert(std::holds_alternative<Json>(imageJsonOrError));

  std::get<Json>(jsonOrError).update(std::get<Json>(imageJsonOrError));

  saveAsJson(projectPath, std::get<Json>(jsonOrError));
}

} // namespace PB
