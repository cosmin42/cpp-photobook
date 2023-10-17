#include <pb/persistence/ProjectDetails.h>

namespace PB {
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