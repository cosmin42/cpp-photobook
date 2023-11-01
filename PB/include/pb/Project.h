#pragma once

#include <boost/functional/hash.hpp>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>

#include <pb/Config.h>
#include <pb/persistence/ProjectMetadata.h>
#include <pb/persistence/ProjectDetails.h>
#include <pb/util/Traits.h>

namespace PB {

class Project final {
public:
  Project() = default;

  Project(Path centralPersistencePath)
  {
    mProjectDetails.uuid(boost::uuids::random_generator()());
    mProjectDetails.supportDirName(boost::uuids::to_string(mProjectDetails.uuid()));
    mProjectDetails.parentDirectory(centralPersistencePath);

    mProjectDetails.setPaperSettings(Context::A4_LANDSCAPE_PAPER);
  }

  explicit Project(ProjectDetails const &projectDetails)
      : mProjectDetails(projectDetails)
  {
  }

  ~Project() = default;

  ProjectDetails &details() { return mProjectDetails; }

  void updateProjectName(std::string name)
  {
    mProjectDetails.supportDirName(name);
  }

  void updateProjectPath(Path path) { mProjectDetails.parentDirectory(path); }

  std::pair<std::string, std::string> locationData()
  {
    auto uuidStr = boost::uuids::to_string(mProjectDetails.uuid());
    auto pathStr = mProjectDetails.parentDirectory().string();

    return std::pair<std::string, std::string>(uuidStr, pathStr);
  }

private:
  ProjectDetails mProjectDetails;
};

class ProjectsSet {
public:
  Project create(Path centralPersistencePath)
  {
    Project newProject(centralPersistencePath);
    mSet[newProject.details().uuid()] = newProject;
    return newProject;
  }

  std::optional<Error>
  load(std::unordered_map<std::string, std::string> const &map)
  {
    for (auto &[uuid, path] : map) {
    }
  }

  std::unordered_map<boost::uuids::uuid, Project,
                     boost::hash<boost::uuids::uuid>> &
  set()
  {
    return mSet;
  }

private:
  std::unordered_map<boost::uuids::uuid, Project,
                     boost::hash<boost::uuids::uuid>>
      mSet;
};

} // namespace PB