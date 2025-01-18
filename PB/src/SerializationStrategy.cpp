#include <pb/components/SerializationStrategy.h>

#include <pb/components/SerializationStrategy.h>
#include <pb/entities/CollageImage.h>
#include <pb/entities/GenericImage.h>
#include <pb/entities/RegularImageV2.h>
#include <pb/entities/TextImageV2.h>
#include <pb/project/Project.h>

#include <boost/bimap/bimap.hpp>
#include <magic_enum/magic_enum.hpp>

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

template <> std::variant<ImageType, PBDev::Error> deserialize(Json jsonData)
{
  auto data = jsonData.get<std::string>();

  auto imageType = magic_enum::enum_cast<ImageType>(data);

  if (!imageType.has_value()) {
    return PBDev::Error() << ErrorCode::JSONParseError;
  }

  return imageType.value();
}

template <>
std::variant<GenericImagePtr, PBDev::Error> deserialize(Json jsonData)
{
  auto imageTypeOrError = deserialize<ImageType>(jsonData.at("type"));

  if (std::holds_alternative<PBDev::Error>(imageTypeOrError)) {
    return std::get<PBDev::Error>(imageTypeOrError);
  }
  auto imageType = std::get<ImageType>(imageTypeOrError);

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
      jsonData, "paperSettings", PaperSettings(), true);
  if (std::holds_alternative<PBDev::Error>(paperSettingsOrError)) {
    return std::get<PBDev::Error>(paperSettingsOrError);
  }
  projectDetails.paperSettings = std::get<PaperSettings>(paperSettingsOrError);

  auto nameOrError = deserialize<std::string>(jsonData, "name");
  if (std::holds_alternative<PBDev::Error>(nameOrError)) {
    return std::get<PBDev::Error>(nameOrError);
  }
  projectDetails.name = std::get<std::string>(nameOrError);

  auto iamgeMonitorOrError =
      deserialize<std::vector<std::vector<GenericImagePtr>>>(
          jsonData, "imageMonitor",
          std::vector<std::vector<GenericImagePtr>>());

  if (std::holds_alternative<PBDev::Error>(iamgeMonitorOrError)) {
    return std::get<PBDev::Error>(iamgeMonitorOrError);
  }

  // TODO: The roots should be decoupled from the path which is not relevant in
  // general as the images are copied to the project folder
  // Then, the root should be packaged along with the line in the image monitor
  // line.
  auto rootsOrError =
      deserializeSpecial(jsonData, "roots", std::vector<Path>());

  if (std::holds_alternative<PBDev::Error>(rootsOrError)) {
    return std::get<PBDev::Error>(rootsOrError);
  }

  auto &imageMonitor =
      std::get<std::vector<std::vector<GenericImagePtr>>>(iamgeMonitorOrError);
  auto &roots = std::get<std::vector<Path>>(rootsOrError);

  PBDev::basicAssert(imageMonitor.size() == roots.size());

  for (auto i = 0; i < roots.size(); ++i) {
    projectDetails.imageMonitor()->addRow(roots[i], imageMonitor[i]);
  }

  auto stagedImagesOrError = deserialize<std::vector<GenericImagePtr>>(
      jsonData, "stagedImages", std::vector<GenericImagePtr>());

  if (std::holds_alternative<PBDev::Error>(stagedImagesOrError)) {
    return std::get<PBDev::Error>(stagedImagesOrError);
  }
  projectDetails.stagedImages()->addPictures(
      std::get<std::vector<GenericImagePtr>>(stagedImagesOrError));

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
