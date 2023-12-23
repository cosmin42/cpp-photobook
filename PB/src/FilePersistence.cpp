#include <pb/persistence/FilePersistence.h>

#include <fstream>

namespace PB {
FilePersistence::FilePersistence(Path path) : mPath(path) {}

void FilePersistence::read(
    std::function<void(std::variant<Json, PBDev::Error>)> onReturn)
{
  std::ifstream file(mPath);
  if (!file.is_open()) {
    onReturn(PBDev::Error() << ErrorCode::FileDoesNotExist);
    return;
  }
  Json jsonData;
  try {
    file >> jsonData;
    file.close();
    onReturn(jsonData);
  }
  catch (Json::exception err) {
    onReturn(PBDev::Error() << ErrorCode::JSONParseError << err.what());
  }
}

void FilePersistence::write(Json                                      jsonData, std::function<void(std::optional<PBDev::Error>)> onReturn)
{
  std::ofstream outputFile(mPath.string());

  if (!outputFile.is_open()) {
    onReturn(PBDev::Error() << ErrorCode::FileDoesNotExist);
    return;
  }

  static constexpr int INDENTATION_SIZE = 4;

  try {
    outputFile << jsonData.dump(INDENTATION_SIZE);
    outputFile.close();
    onReturn(std::nullopt);
  }
  catch (Json::exception &err) {
    onReturn(PBDev::Error() << ErrorCode::JSONParseError << err.what());
  }
}

std::variant<std::unordered_map<std::string, std::string>, PBDev::Error>
FilePersistence::parseData(std::string const &rawData)
{
  std::unordered_map<std::string, std::string> parsed;
  auto                     tokensRanges = rawData | std::views::split('\n');
  std::vector<std::string> pair;
  for (const auto &tokenRange : tokensRanges) {
    auto newStr = std::string(tokenRange.begin(), tokenRange.end());
    if (newStr.empty()) {
      continue;
    }
    pair.push_back(newStr);
    if (pair.size() == 2) {
      parsed[pair.at(0)] = pair.at(1);
      pair.clear();
    }
  }
  if (pair.size() == 1) {
    return PBDev::Error() << ErrorCode::CorruptPersistenceFile;
  }
  return parsed;
}
} // namespace PB