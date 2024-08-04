#include <pb/persistence/SerializationStrategy.h>

#include <pb/image/RegularImage.h>
#include <pb/image/TextImage.h>
#include <pb/image/VirtualImage.h>
#include <pb/project/Project.h>

#include <boost/bimap/bimap.hpp>
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
#ifndef _CLANG_UML_
  auto resourcesOrError = deserializeSpecial(jsonData, "resource");
  if (std::holds_alternative<PBDev::Error>(resourcesOrError)) {
    return std::get<PBDev::Error>(resourcesOrError);
  }
#endif
  auto frontendFull = std::get<Path>(frontendFullOrError);
  auto frontendMedium = std::get<Path>(frontendMediumOrError);
  auto frontendSmall = std::get<Path>(frontendSmallOrError);
  auto processingFinished = std::get<bool>(processingFinishedOrError);

#ifndef _CLANG_UML_
  if (imageType == "Regular") {
    auto imagePtr = std::make_shared<RegularImage>(
        ImageResources{frontendFull, frontendMedium, frontendSmall},
        processingFinished, std::get<std::vector<Path>>(resourcesOrError));
    return std::dynamic_pointer_cast<VirtualImage>(imagePtr);
  }
  else if (imageType == "Text") {
    auto imagePtr = std::make_shared<TextImage>(
        ImageResources{frontendFull, frontendMedium, frontendSmall},
        processingFinished, std::get<std::vector<Path>>(resourcesOrError));
    return std::dynamic_pointer_cast<VirtualImage>(imagePtr);
  }
  else {
    PBDev::basicAssert(false);
  }
#endif
  return nullptr;
}

template <>
std::variant<std::vector<std::vector<std::shared_ptr<VirtualImage>>>,
             PBDev::Error>
deserialize(Json jsonData)
{
  std::vector<std::vector<std::shared_ptr<VirtualImage>>> result;

  for (auto &jsonLineData : jsonData) {
    std::vector<std::shared_ptr<VirtualImage>> line;
    for (auto &jsonElement : jsonLineData) {
      auto imageOrError =
          deserialize<std::shared_ptr<VirtualImage>>(jsonElement);
      if (std::holds_alternative<PBDev::Error>(imageOrError)) {
        return std::get<PBDev::Error>(imageOrError);
      }
      auto virtualImage = std::get<std::shared_ptr<VirtualImage>>(imageOrError);
      line.push_back(virtualImage);
    }
    result.push_back(line);
  }

  return result;
}

template <>
std::variant<std::vector<std::shared_ptr<VirtualImage>>, PBDev::Error>
deserialize(Json jsonData)
{
  std::vector<std::shared_ptr<VirtualImage>> line;
  for (auto &jsonElement : jsonData) {
    auto imageOrError = deserialize<std::shared_ptr<VirtualImage>>(jsonElement);
    if (std::holds_alternative<PBDev::Error>(imageOrError)) {
      return std::get<PBDev::Error>(imageOrError);
    }
    auto virtualImage = std::get<std::shared_ptr<VirtualImage>>(imageOrError);
    line.push_back(virtualImage);
  }

  return line;
}

template <> std::variant<Project, PBDev::Error> deserialize(Json jsonData)
{
  Project projectDetails;

  auto paperSettingsOrError = deserialize<PaperSettings>(
      jsonData.at("root"), "paper-settings", PaperSettings(), true);
  if (std::holds_alternative<PBDev::Error>(paperSettingsOrError)) {
    return std::get<PBDev::Error>(paperSettingsOrError);
  }
  projectDetails.paperSettings = std::get<PaperSettings>(paperSettingsOrError);

  return projectDetails;
}

std::variant<std::vector<Path>, PBDev::Error> deserializeSpecial(Json jsonData)
{
  std::vector<Path> result;
  for (const auto &stagedFolderJson : jsonData) {
    auto deserializedOrError = deserialize<Path>(stagedFolderJson);
    if (std::holds_alternative<PBDev::Error>(deserializedOrError)) {
      return std::get<PBDev::Error>(deserializedOrError);
    }
    else {
      result.push_back(std::get<Path>(deserializedOrError));
    }
  }
  return result;
}

std::variant<std::vector<Path>, PBDev::Error>
deserializeSpecial(Json jsonData, std::string key,
                   std::vector<Path> defaultValue, bool optional)
{
  if (jsonData.contains(key)) {
    return deserializeSpecial(jsonData.at(key));
  }
  if (optional) {
    return defaultValue;
  }
  return PBDev::Error() << ErrorCode::JSONParseError;
}

template <>
std::variant<Json, PBDev::Error>
serialize(int depth, std::pair<std::string, boost::uuids::uuid> const &entry)
{
  Json json;
  json[entry.first] = boost::uuids::to_string(entry.second);
  spdlog::info("{}(string, uuid) {}\n", std::string(depth * 2, ' '),
               json.dump());
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
  spdlog::info("{}(string, vector) {}\n", std::string(depth * 2, ' '),
               json.dump());
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

  spdlog::info("{}(bimap<Path, string>) {}\n", std::string(depth * 2, ' '),
               json.dump());
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
  spdlog::info("{}(string, PaperSettings) {}\n", std::string(depth * 2, ' '),
               json.dump());
  return json;
}

template <>
std::variant<Json, PBDev::Error>
serialize(int depth, std::pair<std::string, Project> const &entry)
{
  auto [key, projectDetails] = entry;

  auto jsonOrError = serialize<PaperSettings>(
      depth + 1, {"paper-settings", projectDetails.paperSettings});

  if (std::holds_alternative<PBDev::Error>(jsonOrError)) {
    return jsonOrError;
  }

  Json json;
  json[key] = std::get<Json>(jsonOrError);
  spdlog::info("{}(string, Project) {}\n", std::string(depth * 2, ' '),
               json.dump());
  return json;
}

template <>
std::variant<Json, PBDev::Error>
serialize(int depth, std::pair<std::string, VirtualImageType> const &entry)
{
  auto [key, imageType] = entry;

  Json json;

  json[key] = magic_enum::enum_name(imageType);

  spdlog::info("%s(string, RegularImage) %s\n",
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
  auto jsonOrError =
      serialize<VirtualImageType, Path, Path, Path, std::vector<Path>, bool>(
          depth + 1, {"img-type", image->type()},
          {"frontend-full", image->frontend().full},
          {"frontend-medium", image->frontend().medium},
          {"frontend-small", image->frontend().small}, {"resource", resources},
          {"processing-finished", image->processed()});

  if (std::holds_alternative<PBDev::Error>(jsonOrError)) {
    return jsonOrError;
  }

  Json json;
  json[key] = std::get<Json>(jsonOrError);
  spdlog::info("%s(string, RegularImage) %s\n",
               std::string(depth * 2, ' ').c_str(), json.dump());
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
