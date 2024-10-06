#include <pb/components/SerializationStrategy.h>

#include <pb/entities/GenericImage.h>
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
std::variant<GenericImagePtr, PBDev::Error> deserialize(Json jsonData)
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
  //auto processingFinished = std::get<bool>(processingFinishedOrError);

#ifndef _CLANG_UML_
  /*
  if (imageType == "Regular") {
    auto imagePtr = std::make_shared<RegularImage>(
        ImageResources{frontendFull, frontendMedium, frontendSmall},
        processingFinished, std::get<std::vector<Path>>(resourcesOrError));
    return std::dynamic_pointer_cast<GenericImagePtr>(imagePtr);
  }
  else if (imageType == "Text") {
    auto imagePtr = std::make_shared<TextImage>(
        ImageResources{frontendFull, frontendMedium, frontendSmall},
        processingFinished, std::get<std::vector<Path>>(resourcesOrError));
    return std::dynamic_pointer_cast<GenericImagePtr>(imagePtr);
  }
  else {
    PBDev::basicAssert(false);
  }
  */
#else
  UNUSED(processingFinished);
#endif
  return nullptr;
}

template <>
std::variant<std::vector<std::vector<GenericImagePtr>>, PBDev::Error>
deserialize(Json jsonData)
{
  std::vector<std::vector<GenericImagePtr>> result;
  /*
  for (auto &jsonLineData : jsonData) {
    std::vector<std::shared_ptr<GenericImagePtr>> line;
    for (auto &jsonElement : jsonLineData) {
      auto imageOrError =
          deserialize<std::shared_ptr<GenericImagePtr>>(jsonElement);
      if (std::holds_alternative<PBDev::Error>(imageOrError)) {
        return std::get<PBDev::Error>(imageOrError);
      }
      auto GenericImagePtr =
  std::get<std::shared_ptr<GenericImagePtr>>(imageOrError);
      line.push_back(GenericImagePtr);
    }
    result.push_back(line);
  }
  */
  return result;
}

template <>
std::variant<std::vector<GenericImagePtr>, PBDev::Error>
deserialize(Json jsonData)
{
  std::vector<GenericImagePtr> line;
  /*
  for (auto &jsonElement : jsonData) {
    auto imageOrError =
  deserialize<std::shared_ptr<GenericImagePtr>>(jsonElement); if
  (std::holds_alternative<PBDev::Error>(imageOrError)) { return
  std::get<PBDev::Error>(imageOrError);
    }
    auto GenericImagePtr =
  std::get<std::shared_ptr<GenericImagePtr>>(imageOrError);
    line.push_back(GenericImagePtr);
  }
  */
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

} // namespace PB::Text
