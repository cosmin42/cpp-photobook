#include <pb/persistence/ProjectDetails.h>

namespace PB {

std::optional<Error> ProjectDetails::check(Json const &jsonData,
                                           std::string key)
{
  if (!jsonData.contains(key)) {
    return Error() << ErrorCode::JSONParseError << key;
  }
  return std::nullopt;
}

std::optional<Error> ProjectDetails::check(Json const &jsonData,
                                           std::vector<std::string> const keys)
{
  for (auto &key : keys) {
    auto maybeError = check(jsonData, key);
    if (maybeError) {
      return maybeError.value();
    }
  }
  return std::nullopt;
}

std::variant<ProjectDetails, Error> ProjectDetails::parse(Json const &jsonData)
{
  auto maybeError =
      check(jsonData, {"project-uuid", "project-name", "project-patch",
                       "imported-folders", "staged-images"});

  if (maybeError) {
    return maybeError.value();
  }
  ProjectDetails projectDetails;

  auto mUuidStr = jsonData.at("project-uuid").dump();
  auto generator = boost::uuids::string_generator();
  projectDetails.mUuid = generator(mUuidStr);

  projectDetails.mSupportDirName = jsonData.at("project-name").dump();

  projectDetails.mParentDirectory = Path(jsonData.at("project-path").dump());

  auto &importedFoldersJson = jsonData.at("imported-folders");

  for (const auto &importedFolderJson : importedFoldersJson) {
    projectDetails.mImportedPaths.push_back(
        Path(importedFolderJson["path"].dump()));
  }

  auto &stagedFoldersJson = jsonData.at("staged-images");

  for (const auto &stagedFolderJson : stagedFoldersJson) {
    projectDetails.mStagedImages.push_back(Path(stagedFolderJson.dump()));
  }

  return projectDetails;
}

Path ProjectDetails::supportFolder() const
{
  return mParentDirectory / mSupportDirName;
}

Path ProjectDetails::projectFile() const
{
  return mParentDirectory / (mSupportDirName + Context::BOOK_EXTENSION);
}

ProjectDetails::operator std::unordered_map<std::string, std::string>() const
{
  std::unordered_map<std::string, std::string> result;

  result["project-uuid"] = boost::uuids::to_string(mUuid);
  result["project-name"] = mSupportDirName;
  result["project-path"] = mParentDirectory.string();

  return result;
}

ProjectDetails::operator Json() const
{
  Json jsonData;
  jsonData["project-uuid"] = boost::uuids::to_string(mUuid);
  jsonData["project-name"] = mSupportDirName;
  jsonData["project-path"] = mParentDirectory.string();

  Json importedFolders = {};

  for (auto i = 0; i < mImportedPaths.size(); ++i) {
    Json pathFlagPair;
    pathFlagPair["path"] = mImportedPaths.at(i).string();
    pathFlagPair["thumbnails-generated"] = false;
    importedFolders.push_back(pathFlagPair);
  }

  jsonData["imported-folders"] = importedFolders;

  Json stagedImagesArray = {};
  for (auto i = 0; i < mStagedImages.size(); ++i) {
    stagedImagesArray.push_back(mStagedImages.at(i).string());
  }

  jsonData["staged-images"] = stagedImagesArray;
  return jsonData;
}
} // namespace PB