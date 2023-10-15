#include <pb/Project.h>

namespace PB {

std::variant<ProjectDetails, Error>
convert(std::unordered_map<std::string, std::string> const &map)
{
  ProjectDetails projectDetails;

  auto projectUUID = PB::mapGet<std::unordered_map, std::string, std::string>(
      map, std::string("project-uuid"));

  if (!projectUUID) {
    return Error() << ErrorCode::InvalidProjectDescription;
  }

  auto projectName = PB::mapGet<std::unordered_map, std::string, std::string>(
      map, std::string("project-name"));

  if (!projectName) {
    return Error() << ErrorCode::InvalidProjectDescription;
  }

  auto projectPath = PB::mapGet<std::unordered_map, std::string, std::string>(
      map, std::string("project-path"));

  if (!projectPath) {
    return Error() << ErrorCode::InvalidProjectDescription;
  }

  boost::uuids::uuid newUUID;

  try {
    boost::uuids::string_generator gen;
    newUUID = gen(*projectUUID);
  }
  catch (...) {
    return Error() << ErrorCode::InvalidUUID;
  }

  Path newPath = *projectPath;

  if (!std::filesystem::exists(newPath)) {
    return Error() << ErrorCode::ProjectPathDoesNotExist;
  }

  projectDetails.uuid = newUUID;
  projectDetails.supportDirName = projectName.value();
  projectDetails.parentDirectory = newPath;

  return projectDetails;
}

} // namespace PB