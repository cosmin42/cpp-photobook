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

void ProjectDetails::setStagedImages(std::vector<Path> stagedImages)
{
  mStagedImages.clear();
  for (auto &image : stagedImages) {
    mStagedImages.push_back(image);
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