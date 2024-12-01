#include <pb/components/SerializationStrategy.h>

#include <pb/components/SerializationStrategy.h>
#include <pb/entities/CollageImage.h>
#include <pb/entities/GenericImage.h>
#include <pb/entities/RegularImageV2.h>
#include <pb/entities/TextImageV2.h>
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

  auto imageType = (ImageType)jsonData.at("type").get<unsigned>();

  auto hashOrErrror = deserialize<std::string>(jsonData, "hash");

  if (std::holds_alternative<PBDev::Error>(hashOrErrror)) {
    return std::get<PBDev::Error>(hashOrErrror);
  }

  if (imageType == ImageType::Regular) {
    auto originalOrError = deserialize<Path>(jsonData.at("original"));
    if (std::holds_alternative<PBDev::Error>(originalOrError)) {
      return std::get<PBDev::Error>(originalOrError);
    }

    return std::make_shared<RegularImageV2>(std::get<std::string>(hashOrErrror),
                                            std::get<Path>(originalOrError));
  }
  else if (imageType == ImageType::Text) {
    auto textOrError = deserialize<std::string>(jsonData.at("text"));
    if (std::holds_alternative<PBDev::Error>(textOrError)) {
      return std::get<PBDev::Error>(textOrError);
    }

    return std::make_shared<TextImageV2>(std::get<std::string>(hashOrErrror),
                                         std::get<std::string>(textOrError));
  }
  else if (imageType == ImageType::Collage) {
    auto imagesOrError = deserializeSpecial(jsonData.at("images"));
    if (std::holds_alternative<PBDev::Error>(imagesOrError)) {
      return std::get<PBDev::Error>(imagesOrError);
    }

    return std::make_shared<CollageImage>(
        std::get<std::string>(hashOrErrror),
        std::get<std::vector<Path>>(imagesOrError));
  }
  else {
    PBDev::basicAssert(false);
  }
  return PBDev::Error() << "Unknown image type";
}

template <>
std::variant<std::vector<std::vector<GenericImagePtr>>, PBDev::Error>
deserialize(Json jsonData)
{
  std::vector<std::vector<GenericImagePtr>> result;
  for (auto &jsonLineData : jsonData) {
    std::vector<GenericImagePtr> line;
    for (auto &jsonElement : jsonLineData) {
      auto imageOrError = deserialize<GenericImagePtr>(jsonElement);
      if (std::holds_alternative<PBDev::Error>(imageOrError)) {
        return std::get<PBDev::Error>(imageOrError);
      }
      line.push_back(std::get<GenericImagePtr>(imageOrError));
    }
    result.push_back(line);
  }
  return result;
}

template <>
std::variant<std::vector<GenericImagePtr>, PBDev::Error>
deserialize(Json jsonData)
{
  std::vector<GenericImagePtr> line;
  for (auto &jsonElement : jsonData) {
    auto imageOrError = deserialize<GenericImagePtr>(jsonElement);
    if (std::holds_alternative<PBDev::Error>(imageOrError)) {
      return std::get<PBDev::Error>(imageOrError);
    }
    line.push_back(std::get<GenericImagePtr>(imageOrError));
  }
  return line;
}

template <> std::variant<Project, PBDev::Error> deserialize(Json jsonData)
{
  Project projectDetails;

  auto paperSettingsOrError = deserialize<PaperSettings>(
      jsonData, "paper-settings", PaperSettings(), true);
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
