#include <pb/persistence/SerializationStrategy.h>

#include <pb/project/Project.h>

#include <magic_enum.hpp>

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
  auto            x = jsonData.dump();
  ProjectSnapshot projectDetails;

  auto paperSettingsOrError = deserialize<PaperSettings>(
      jsonData, "paper-settings", PaperSettings(), true);
  if (std::holds_alternative<PBDev::Error>(paperSettingsOrError)) {
    return std::get<PBDev::Error>(paperSettingsOrError);
  }
  projectDetails.paperSettings = std::get<PaperSettings>(paperSettingsOrError);

  auto pathCacheOrError =
      deserialize<PathCache>(jsonData, "path-cache", PathCache(), true);
  if (std::holds_alternative<PBDev::Error>(pathCacheOrError)) {
    return std::get<PBDev::Error>(pathCacheOrError);
  }

  projectDetails.pathCache = std::get<PathCache>(pathCacheOrError);

  return projectDetails;
}

template <>
std::variant<std::shared_ptr<VirtualImage>, PBDev::Error>
deserialize(Json jsonData)
{
  auto imageType = jsonData.at("img-type").get<std::string>();

  PBDev::basicAssert(!imageType.empty());

  auto frontendFullOrError = deserialize<Path>(jsonData, "frontend-full");
  if (std::holds_alternative<PBDev::Error>(frontendFullOrError)) {
    return std::get<PBDev::Error>(frontendFullOrError);
  }

  auto frontendMediumOrError = deserialize<Path>(jsonData, "frontend-medium");
  if (std::holds_alternative<PBDev::Error>(frontendMediumOrError)) {
    return std::get<PBDev::Error>(frontendMediumOrError);
  }

  auto frontendSmallOrError = deserialize<Path>(jsonData, "frontend-small");
  if (std::holds_alternative<PBDev::Error>(frontendSmallOrError)) {
    return std::get<PBDev::Error>(frontendSmallOrError);
  }

  auto processingFinishedOrError =
      deserialize<bool>(jsonData, "processing-finished");
  if (std::holds_alternative<PBDev::Error>(processingFinishedOrError)) {
    return std::get<PBDev::Error>(processingFinishedOrError);
  }

  auto resourcesOrError = deserialize<std::vector, Path>(jsonData, "resource");
  if (std::holds_alternative<PBDev::Error>(resourcesOrError)) {
    return std::get<PBDev::Error>(resourcesOrError);
  }

  auto frontendFull = std::get<Path>(frontendFullOrError);
  auto frontendMedium = std::get<Path>(frontendMediumOrError);
  auto frontendSmall = std::get<Path>(frontendSmallOrError);
  auto processingFinished = std::get<bool>(processingFinishedOrError);

  if (imageType == "Regular") {
    auto imagePtr = std::make_shared<RegularImage>(
        frontendFull, frontendMedium, frontendSmall, processingFinished,
        std::get<std::vector<Path>>(resourcesOrError));
    return std::dynamic_pointer_cast<VirtualImage>(imagePtr);
  }
  else if (imageType == "Text") {
    auto imagePtr = std::make_shared<TextImage>(
        frontendFull, frontendMedium, frontendSmall, processingFinished,
        std::get<std::vector<Path>>(resourcesOrError));
    return std::dynamic_pointer_cast<VirtualImage>(imagePtr);
  }
  else {
    PBDev::basicAssert(false);
  }
  return nullptr;
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
      serialize<std::vector<Path>, std::vector<Path>, PaperSettings, PathCache>(
          depth + 1, {"imported-folders", projectDetails.importedPaths},
          {"staged-images", projectDetails.stagedImages},
          {"paper-settings", projectDetails.paperSettings},
          {"path-cache", projectDetails.pathCache});

  if (std::holds_alternative<PBDev::Error>(jsonOrError)) {
    return jsonOrError;
  }

  Json json;
  json[key] = std::get<Json>(jsonOrError);
  PB::printDebug("%s(string, ProjectSnapshot) %s\n",
                 std::string(depth * 2, ' ').c_str(), json.dump().c_str());
  return json;
}

template <>
std::variant<Json, PBDev::Error>
serialize(int depth, std::pair<std::string, VirtualImageType> const &entry)
{
  auto [key, imageType] = entry;

  Json json;

  json[key] = magic_enum::enum_name(imageType);

  PB::printDebug("%s(string, RegularImage) %s\n",
                 std::string(depth * 2, ' ').c_str(), json.dump().c_str());

  return json;
}

// TODO: Find alternative to shared_ptr const&
template <>
std::variant<Json, PBDev::Error>
serialize(int                                                          depth,
          std::pair<std::string, std::shared_ptr<VirtualImage>> const &entry)
{
  auto [key, image] = entry;

  auto resources = std::vector<Path>{image->resources()};
  auto jsonOrError = serialize<VirtualImageType, Path, Path, Path, Path,
                               std::vector<Path>, bool>(
      depth + 1, {"img-type", image->type()}, {"key-path", image->keyPath()},
      {"frontend-full", image->frontend().full},
      {"frontend-medium", image->frontend().medium},
      {"frontend-small", image->frontend().small}, {"resource", resources},
      {"processing-finished", image->processed()});

  if (std::holds_alternative<PBDev::Error>(jsonOrError)) {
    return jsonOrError;
  }

  Json json;
  json[key] = std::get<Json>(jsonOrError);
  PB::printDebug("%s(string, RegularImage) %s\n",
                 std::string(depth * 2, ' ').c_str(), json.dump().c_str());
  return json;
}

template <>
std::variant<Json, PBDev::Error> serialize(
    int depth,
    std::pair<std::string,
              std::vector<std::vector<std::shared_ptr<VirtualImage>>>> const
        &entry)
{

  auto [key, imageMatrix] = entry;

  Json matrix;

  matrix[key];

  for (int i = 0; i < imageMatrix.size(); ++i) {
    Json line;
    for (int j = 0; j < imageMatrix.at(i).size(); ++j) {
      auto jasonOrError = serialize<std::shared_ptr<VirtualImage>>(
          depth + 1, {"placeholder", imageMatrix.at(i).at(j)});
      if (std::holds_alternative<PBDev::Error>(jasonOrError)) {
        return jasonOrError;
      }
      line.push_back(std::get<Json>(jasonOrError)["placeholder"]);
    }

    matrix[key].push_back(line);
  }

  return matrix;
}

template <>
std::variant<Json, PBDev::Error>
serialize(int                                                          depth,
          std::pair<std::string,
                    std::vector<std::shared_ptr<VirtualImage>>> const &entry)
{

  auto [key, imageMatrix] = entry;

  Json line;

  line[key];

  for (int i = 0; i < imageMatrix.size(); ++i) {
    auto jasonOrError = serialize<std::shared_ptr<VirtualImage>>(
        depth + 1, {"placeholder", imageMatrix.at(i)});
    if (std::holds_alternative<PBDev::Error>(jasonOrError)) {
      return jasonOrError;
    }
    line[key].push_back(std::get<Json>(jasonOrError)["placeholder"]);
  }

  return line;
}

} // namespace PB::Text
