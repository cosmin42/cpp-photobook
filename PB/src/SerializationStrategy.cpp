#include <pb/persistence/SerializationStrategy.h>

#include <pb/project/Project.h>

namespace PB::Text {

template <> std::variant<Path, PBDev::Error> deserialize(Json jsonData)
{
  try {
    return Path(jsonData.get<std::string>());
  }
  catch (...) {
    return PBDev::Error() << ErrorCode::JSONParseError;
  }
}

template <> std::variant<int, PBDev::Error> deserialize(Json jsonData)
{
  try {
    return jsonData.get<int>();
  }
  catch (...) {
    return PBDev::Error() << ErrorCode::JSONParseError;
  }
}

template <> std::variant<bool, PBDev::Error> deserialize(Json jsonData)
{
  try {
    return jsonData.get<bool>();
  }
  catch (...) {
    return PBDev::Error() << ErrorCode::JSONParseError;
  }
}

template <> std::variant<std::string, PBDev::Error> deserialize(Json jsonData)
{
  try {
    return jsonData.get<std::string>();
  }
  catch (...) {
    return PBDev::Error() << ErrorCode::JSONParseError;
  }
}

template <>
std::variant<boost::uuids::uuid, PBDev::Error> deserialize(Json jsonData)
{
  try {
    auto               generator = boost::uuids::string_generator();
    boost::uuids::uuid parsedUuid = generator(jsonData.get<std::string>());
    return parsedUuid;
  }
  catch (...) {
    return PBDev::Error() << ErrorCode::JSONParseError;
  }
}

template <> std::variant<PathCache, PBDev::Error> deserialize(Json jsonData)
{
  boost::bimaps::bimap<Path, std::string> entries;
  for (auto it = jsonData.begin(); it != jsonData.end(); ++it) {
    auto pathOrError = deserialize<Path>(it.key());
    if (std::holds_alternative<PBDev::Error>(pathOrError)) {
      return std::get<PBDev::Error>(pathOrError);
    }
    auto hashOrError = deserialize<std::string>(it.value());
    if (std::holds_alternative<PBDev::Error>(hashOrError)) {
      return std::get<PBDev::Error>(hashOrError);
    }
    entries.insert(
        {std::get<Path>(pathOrError), std::get<std::string>(hashOrError)});
  }

  PathCache patchCache(entries);
  return patchCache;
}

template <> std::variant<PaperSettings, PBDev::Error> deserialize(Json jsonData)
{
  PaperSettings paperSettings;

  auto typeOrError = deserialize<int>(jsonData, "type");
  if (std::holds_alternative<PBDev::Error>(typeOrError)) {
    return std::get<PBDev::Error>(typeOrError);
  }
  paperSettings.type = (PaperType)std::get<int>(typeOrError);

  auto ppiOrError = deserialize<int>(jsonData, "ppi");
  if (std::holds_alternative<PBDev::Error>(ppiOrError)) {
    return std::get<PBDev::Error>(ppiOrError);
  }
  paperSettings.ppi = std::get<int>(ppiOrError);

  auto widthOrError = deserialize<int>(jsonData, "width");
  if (std::holds_alternative<PBDev::Error>(widthOrError)) {
    return std::get<PBDev::Error>(widthOrError);
  }
  paperSettings.width = std::get<int>(widthOrError);

  auto heightOrError = deserialize<int>(jsonData, "height");
  if (std::holds_alternative<PBDev::Error>(heightOrError)) {
    return std::get<PBDev::Error>(heightOrError);
  }
  paperSettings.height = std::get<int>(heightOrError);

  return paperSettings;
}

template <>
std::variant<ProjectSnapshot, PBDev::Error> deserialize(Json jsonData)
{
  ProjectSnapshot projectDetails;

  auto uuidOrError = deserialize<boost::uuids::uuid>(jsonData, "project-uuid");
  if (std::holds_alternative<PBDev::Error>(uuidOrError)) {
    return std::get<PBDev::Error>(uuidOrError);
  }
  projectDetails.uuid(std::get<boost::uuids::uuid>(uuidOrError));

  auto nameOrError = deserialize<std::string>(jsonData, "project-name");
  if (std::holds_alternative<PBDev::Error>(nameOrError)) {
    return std::get<PBDev::Error>(nameOrError);
  }
  projectDetails.name(std::get<std::string>(nameOrError));

  auto parentPathOrError = deserialize<std::string>(jsonData, "project-path");
  if (std::holds_alternative<PBDev::Error>(parentPathOrError)) {
    return std::get<PBDev::Error>(parentPathOrError);
  }
  projectDetails.parentDirectory(std::get<std::string>(parentPathOrError));
#ifdef _CLANG_UML_
#else
  auto importedFoldersOrError =
      deserialize<std::vector, Path>(jsonData, "imported-folders");
  if (std::holds_alternative<PBDev::Error>(importedFoldersOrError)) {
    return std::get<PBDev::Error>(importedFoldersOrError);
  }
  projectDetails.setImportedPaths(
      std::get<std::vector<Path>>(importedFoldersOrError));

  auto stagedImagesOrError =
      deserialize<std::vector, Path>(jsonData, "staged-images");
  if (std::holds_alternative<PBDev::Error>(stagedImagesOrError)) {
    return std::get<PBDev::Error>(stagedImagesOrError);
  }
  projectDetails.setStagedImages(
      std::get<std::vector<Path>>(stagedImagesOrError));
#endif
  auto paperSettingsOrError = deserialize<PaperSettings>(
      jsonData, "paper-settings", PaperSettings(), true);
  if (std::holds_alternative<PBDev::Error>(paperSettingsOrError)) {
    return std::get<PBDev::Error>(paperSettingsOrError);
  }
  projectDetails.setPaperSettings(
      std::get<PaperSettings>(paperSettingsOrError));

  auto pathCacheOrError =
      deserialize<PathCache>(jsonData, "path-cache", PathCache(), true);
  if (std::holds_alternative<PBDev::Error>(pathCacheOrError)) {
    return std::get<PBDev::Error>(pathCacheOrError);
  }

  projectDetails.pathCache() = std::get<PathCache>(pathCacheOrError);

  return projectDetails;
}

template <>
std::variant<Json, PBDev::Error>
serialize(int depth, std::pair<std::string, boost::uuids::uuid> const &entry)
{
  Json json;
  json[entry.first] = boost::uuids::to_string(entry.second);
  PB::printDebug("%s(string, uuid) %s\n", std::string(depth * 2, ' ').c_str(),
                 json.dump().c_str());
  return json;
}

template <>
std::variant<Json, PBDev::Error>
serialize(int depth, std::pair<std::string, std::vector<Path>> const &entry)
{
  Json json;
  json[entry.first];
  for (auto &content : entry.second) {
    auto jasonOrError = serialize<Path>(depth + 1, content);
    if (std::holds_alternative<PBDev::Error>(jasonOrError)) {
      return jasonOrError;
    }
    json[entry.first].push_back(std::get<Json>(jasonOrError));
  }
  PB::printDebug("%s(string, vector) %s\n", std::string(depth * 2, ' ').c_str(),
                 json.dump().c_str());
  return json;
}

template <>
std::variant<Json, PBDev::Error>
serialize(int depth,
          std::pair<std::string, boost::bimaps::bimap<Path, std::string>> const
              &entry)
{
  Json json;
  json[entry.first];

  for (auto bimapEntry : entry.second) {
    json[entry.first][bimapEntry.left.string()] = bimapEntry.right;
  }

  PB::printDebug("%s(bimap<Path, string>) %s\n",
                 std::string(depth * 2, ' ').c_str(), json.dump().c_str());
  return json;
}

template <>
std::variant<Json, PBDev::Error>
serialize(int depth, std::pair<std::string, PaperSettings> const &entry)
{
  auto &[key, paperSettings] = entry;

  auto jsonOrError = serialize<int, int, int, int>(
      depth + 1, {"type", (int)paperSettings.type}, {"ppi", paperSettings.ppi},
      {"width", paperSettings.width}, {"height", paperSettings.height});

  if (std::holds_alternative<PBDev::Error>(jsonOrError)) {
    return jsonOrError;
  }
  Json json;
  json[key] = std::get<Json>(jsonOrError);
  PB::printDebug("%s(string, PaperSettings) %s\n",
                 std::string(depth * 2, ' ').c_str(), json.dump().c_str());
  return json;
}

template <>
std::variant<Json, PBDev::Error>
serialize(int depth, std::pair<std::string, PathCache> const &entry)
{
  auto &[key, pathCache] = entry;
  return serialize<boost::bimaps::bimap<Path, std::string>>(
      depth + 1, {"path-cache", pathCache.data()});
}

template <>
std::variant<Json, PBDev::Error>
serialize(int depth, std::pair<std::string, ProjectSnapshot> const &entry)
{
  auto [key, projectDetails] = entry;

  auto jsonOrError =
      serialize<boost::uuids::uuid, std::string, Path, std::vector<Path>,
                std::vector<Path>, PaperSettings, PathCache>(
          depth + 1, {"project-uuid", projectDetails.uuid()},
          {"project-name", projectDetails.name()},
          {"project-path", projectDetails.parentDirectory()},
          {"imported-folders", projectDetails.importedFolderList()},
          {"staged-images", projectDetails.stagedImagesList()},
          {"paper-settings", projectDetails.paperSettings()},
          {"path-cache", projectDetails.pathCache()});

  if (std::holds_alternative<PBDev::Error>(jsonOrError)) {
    return jsonOrError;
  }

  Json json;
  json[key] = std::get<Json>(jsonOrError);
  PB::printDebug("%s(string, ProjectSnapshot) %s\n",
                 std::string(depth * 2, ' ').c_str(), json.dump().c_str());
  return json;
}

} // namespace PB::Text