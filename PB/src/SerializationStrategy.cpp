#include <pb/SerializationStrategy.h>

#include <pb/Project.h>

namespace PB::Text {

template <> std::variant<Path, Error> deserialize(Json jsonData)
{
  try {
    return Path(jsonData.get<std::string>());
  }
  catch (...) {
    return Error() << ErrorCode::JSONParseError;
  }
}

template <> std::variant<int, Error> deserialize(Json jsonData)
{
  try {
    return jsonData.get<int>();
  }
  catch (...) {
    return Error() << ErrorCode::JSONParseError;
  }
}

template <> std::variant<bool, Error> deserialize(Json jsonData)
{
  try {
    return jsonData.get<bool>();
  }
  catch (...) {
    return Error() << ErrorCode::JSONParseError;
  }
}

template <> std::variant<std::string, Error> deserialize(Json jsonData)
{
  try {
    return jsonData.get<std::string>();
  }
  catch (...) {
    return Error() << ErrorCode::JSONParseError;
  }
}

template <> std::variant<boost::uuids::uuid, Error> deserialize(Json jsonData)
{
  try {
    auto               generator = boost::uuids::string_generator();
    boost::uuids::uuid parsedUuid = generator(jsonData.get<std::string>());
    return parsedUuid;
  }
  catch (...) {
    return Error() << ErrorCode::JSONParseError;
  }
}

template <> std::variant<PaperSettings, Error> deserialize(Json jsonData)
{
  PaperSettings paperSettings;

  auto typeOrError = deserialize<int>(jsonData, "type");
  if (std::holds_alternative<Error>(typeOrError)) {
    return std::get<Error>(typeOrError);
  }
  paperSettings.type = (PaperType)std::get<int>(typeOrError);

  auto ppiOrError = deserialize<int>(jsonData, "ppi");
  if (std::holds_alternative<Error>(ppiOrError)) {
    return std::get<Error>(ppiOrError);
  }
  paperSettings.ppi = std::get<int>(ppiOrError);

  auto widthOrError = deserialize<int>(jsonData, "width");
  if (std::holds_alternative<Error>(widthOrError)) {
    return std::get<Error>(widthOrError);
  }
  paperSettings.width = std::get<int>(widthOrError);

  auto heightOrError = deserialize<int>(jsonData, "height");
  if (std::holds_alternative<Error>(heightOrError)) {
    return std::get<Error>(heightOrError);
  }
  paperSettings.width = std::get<int>(heightOrError);

  return paperSettings;
}

template <> std::variant<ProjectDetails, Error> deserialize(Json jsonData)
{
  ProjectDetails projectDetails;

  auto uuidOrError = deserialize<boost::uuids::uuid>(jsonData, "project-uuid");
  if (std::holds_alternative<Error>(uuidOrError)) {
    return std::get<Error>(uuidOrError);
  }
  projectDetails.uuid(std::get<boost::uuids::uuid>(uuidOrError));

  auto nameOrError = deserialize<std::string>(jsonData, "project-name");
  if (std::holds_alternative<Error>(nameOrError)) {
    return std::get<Error>(nameOrError);
  }
  projectDetails.supportDirName(std::get<std::string>(nameOrError));

  auto parentPathOrError = deserialize<std::string>(jsonData, "project-path");
  if (std::holds_alternative<Error>(parentPathOrError)) {
    return std::get<Error>(parentPathOrError);
  }
  projectDetails.parentDirectory(std::get<std::string>(parentPathOrError));

  auto importedFoldersOrError =
      deserialize<std::vector, Path>(jsonData, "imported-folders");
  if (std::holds_alternative<Error>(importedFoldersOrError)) {
    return std::get<Error>(importedFoldersOrError);
  }
  projectDetails.setImportedPaths(
      std::get<std::vector<Path>>(importedFoldersOrError));

  auto stagedImagesOrError =
      deserialize<std::vector, Path>(jsonData, "staged-images");
  if (std::holds_alternative<Error>(stagedImagesOrError)) {
    return std::get<Error>(stagedImagesOrError);
  }
  projectDetails.setStagedImages(
      std::get<std::vector<Path>>(stagedImagesOrError));

  auto paperSettingsOrError = deserialize<PaperSettings>(
      jsonData, "paper-settings", PaperSettings(), true);
  if (std::holds_alternative<Error>(paperSettingsOrError)) {
    return std::get<Error>(paperSettingsOrError);
  }
  projectDetails.setPaperSettings(
      std::get<PaperSettings>(paperSettingsOrError));
  return projectDetails;
}

} // namespace PB::Text