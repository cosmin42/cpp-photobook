#include <pb/persistence/ProjectDetails.h>

namespace PB {
Path ProjectDetails::supportFolder() const
{
  return parentDirectory / supportDirName;
}

Path ProjectDetails::projectFile() const
{
  return parentDirectory / (supportDirName + Context::BOOK_EXTENSION);
}

ProjectDetails::operator std::unordered_map<std::string, std::string>() const
{
  std::unordered_map<std::string, std::string> result;

  result["project-uuid"] = boost::uuids::to_string(uuid);
  result["project-name"] = supportDirName;
  result["project-path"] = parentDirectory.string();

  return result;
}

ProjectDetails::operator Json() const
{
  Json jsonData;
  jsonData["project-uuid"] = boost::uuids::to_string(uuid);
  jsonData["project-name"] = supportDirName;
  jsonData["project-path"] = parentDirectory.string();

  Json importedFolders = {};

  for (auto i = 0; i < importedPaths.size(); ++i) {
    Json pathFlagPair;
    pathFlagPair["path"] = importedPaths.at(i).string();
    pathFlagPair["thumbnails-generated"] = false;
    importedFolders.push_back(pathFlagPair);
  }

  jsonData["imported-folders"] = importedFolders;

  Json stagedImagesArray = {};
  for (auto i = 0; i < stagedImages.size(); ++i) {
    stagedImagesArray.push_back(stagedImages.at(i).string());
  }

  jsonData["staged-images"] = stagedImagesArray;
  return jsonData;
}
} // namespace PB