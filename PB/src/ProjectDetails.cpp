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

std::variant<ProjectDetails, Error>
ProjectDetails::parseProjectDetails(Json const &jsonData)
{
  auto maybeError =
      check(jsonData, {"project-uuid", "project-name", "project-path",
                       "imported-folders", "staged-images"});
  if (maybeError) {
    return maybeError.value();
  }
  ProjectDetails projectDetails;

  auto mUuidStr = jsonData.at("project-uuid").get<std::string>();
  auto generator = boost::uuids::string_generator();
  projectDetails.mUuid = generator(mUuidStr);

  projectDetails.mSupportDirName =
      jsonData.at("project-name").get<std::string>();

  projectDetails.mParentDirectory =
      Path(jsonData.at("project-path").get<std::string>());

  auto &importedFoldersJson = jsonData.at("imported-folders");
  if (!importedFoldersJson.is_null()) {
    for (const auto &importedFolderJson : importedFoldersJson) {
      projectDetails.mImportedPaths.push_back(
          Path(importedFolderJson["path"].get<std::string>()));
    }
  }

  auto &stagedFoldersJson = jsonData.at("staged-images");
  if (!importedFoldersJson.is_null()) {
    for (const auto &stagedFolderJson : stagedFoldersJson) {
      projectDetails.mStagedImages.push_back(
          Path(stagedFolderJson.get<std::string>()));
    }
  }

  
  if (jsonData.contains("paper-settings")) {
    auto &paperSettingsJson = jsonData.at("paper-settings");
    auto maybePaperSettings = parsePaperSettings(paperSettingsJson);
    if (std::holds_alternative<PB::PaperSettings>(maybePaperSettings)) {
      projectDetails.mPaperSettings =
          std::get<PB::PaperSettings>(maybePaperSettings);
    }
    else {
      PB::printWarning("Paper settings coul not be parsed\n");
    }
  }
  else {
    PB::printWarning("Paper settings not present\n");
  }

  return projectDetails;
}

std::variant<PaperSettings, Error>
ProjectDetails::parsePaperSettings(Json const &jsonData)
{
  auto maybeError = check(jsonData, {"type", "ppi", "width", "height"});
  if (maybeError) {
    return PaperSettings();
  }

  PaperSettings paperSettings;
  paperSettings.type = (PaperType)jsonData.at("type").get<int>();
  paperSettings.ppi = jsonData.at("ppi").get<int>();
  paperSettings.width = jsonData.at("width").get<int>();
  paperSettings.height = jsonData.at("height").get<int>();

  return paperSettings;
}

Path ProjectDetails::supportFolder() const
{
  return mParentDirectory / mSupportDirName;
}

Path ProjectDetails::projectFile() const
{
  return mParentDirectory / (mSupportDirName + Context::BOOK_EXTENSION);
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

  jsonData["paper-settings"] = Json(mPaperSettings);

  return jsonData;
}

void ProjectDetails::setImportedPaths(std::vector<Path> paths)
{
  mImportedPaths.clear();
  for (auto &path : paths) {
    mImportedPaths.push_back(path);
  }
}

void ProjectDetails::setStagedImages(std::vector<Thumbnails> stagedImages)
{
  mStagedImages.clear();
  for (auto &image : stagedImages) {
    mStagedImages.push_back(image.fullPath);
  }
}

void ProjectDetails::setPaperSettings(PaperSettings paperSettings)
{
  mPaperSettings = paperSettings;
}

void ProjectDetails::removeStagedImage(int index)
{
  if (index >= mStagedImages.size()) {
    return;
  }
  mStagedImages.erase(mStagedImages.begin() + index);
}

std::vector<Path> ProjectDetails::importedFolderList() const
{
  return mImportedPaths;
}

std::vector<Path> ProjectDetails::stagedImagesList() const
{
  return mStagedImages;
}

PaperSettings ProjectDetails::paperSettings() const { return mPaperSettings; }

} // namespace PB