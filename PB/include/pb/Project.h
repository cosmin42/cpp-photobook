#pragma once

#include <boost/functional/hash.hpp>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>

#include <pb/Config.h>
#include <pb/util/Traits.h>

namespace PB {
struct ProjectDetails {
  boost::uuids::uuid uuid;
  std::string        name;
  Path               parentDirectory;

  operator std::unordered_map<std::string, std::string>()
  {
    std::unordered_map<std::string, std::string> result;

    result["project-uuid"] = boost::uuids::to_string(uuid);
    result["project-name"] = name;
    result["project-path"] = parentDirectory.string();

    return result;
  }
};

static std::variant<ProjectDetails, Error>
convert(std::unordered_map<std::string, std::string> const &map)
{
  ProjectDetails projectDetails;

  auto projectUUID = PB::mapGet<std::unordered_map, std::string, std::string>(
      map, std::string("project-uuid"));

  if (!projectUUID) {
    return Error() << ErrorKind::InvalidProjectDescription;
  }

  auto projectName = PB::mapGet<std::unordered_map, std::string, std::string>(
      map, std::string("project-name"));

  if (!projectName) {
    return Error() << ErrorKind::InvalidProjectDescription;
  }

  auto projectPath = PB::mapGet<std::unordered_map, std::string, std::string>(
      map, std::string("project-path"));

  if (!projectPath) {
    return Error() << ErrorKind::InvalidProjectDescription;
  }

  boost::uuids::uuid newUUID;

  try {
    boost::uuids::string_generator gen;
    newUUID = gen(*projectUUID);
  }
  catch (...) {
    return Error() << ErrorKind::InvalidUUID;
  }

  Path newPath = *projectPath;

  if (!std::filesystem::exists(newPath)) {
    return Error() << ErrorKind::ProjectPathDoesNotExist;
  }

  projectDetails.uuid = newUUID;
  projectDetails.name = *projectName;
  projectDetails.parentDirectory = newPath;

  return projectDetails;
}

template <typename PersistenceType> class Project final {
public:
  Project()
  {
    mProjectDetails.uuid = boost::uuids::random_generator()();
    mProjectDetails.name =
        boost::uuids::to_string(mProjectDetails.uuid) + ".photobook";
    mProjectDetails.parentDirectory = Persistence<PersistenceType>::localFolder();
  }

  explicit Project(ProjectDetails const &projectDetails)
      : mProjectDetails(projectDetails)
  {
  }

  ~Project() = default;

  ProjectDetails &details() { return mProjectDetails; }

  std::pair<std::string, std::string> locationData()
  {
    auto uuidStr = boost::uuids::to_string(mProjectDetails.uuid);
    auto pathStr = mProjectDetails.parentDirectory.string();

    return std::pair<std::string, std::string>(uuidStr, pathStr);
  }

private:
  ProjectDetails mProjectDetails;
};

template <typename PersistenceType> class ProjectsSet {
public:
  Project<PersistenceType> create()
  {
    Project<PersistenceType> newProject;
    mSet[newProject.details().uuid] = newProject;
    return newProject;
  }

  std::optional<Error>
  load(std::unordered_map<std::string, std::string> const &map)
  {
    for (auto &[uuid, path] : map) {
    }
  }

  std::unordered_map<boost::uuids::uuid, Project<PersistenceType>,
                     boost::hash<boost::uuids::uuid>> &
  set()
  {
    return mSet;
  }

private:
  std::unordered_map<boost::uuids::uuid, Project<PersistenceType>,
                     boost::hash<boost::uuids::uuid>>
      mSet;
};

} // namespace PB