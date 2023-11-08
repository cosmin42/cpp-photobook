#pragma once

#include <boost/functional/hash.hpp>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>

#include <pb/Config.h>
#include <pb/persistence/ProjectDetails.h>
#include <pb/persistence/ProjectMetadata.h>
#include <pb/util/Traits.h>

namespace PB {

class Project final {
public:
  Project() = default;

  Project(Path centralPersistencePath)
  {
    auto uuid = boost::uuids::random_generator()();
    auto projectPath = centralPersistencePath / (boost::uuids::to_string(uuid) +
                                                 Context::BOOK_EXTENSION);

    mMetadata =
        ProjectMetadata(boost::uuids::to_string(uuid), projectPath.string());

    mProjectDetails.uuid(uuid);
    mProjectDetails.supportDirName(
        boost::uuids::to_string(mProjectDetails.uuid()));
    mProjectDetails.parentDirectory(centralPersistencePath);

    mProjectDetails.setPaperSettings(Context::A4_LANDSCAPE_PAPER);
  }

  ~Project() = default;

  ProjectDetails &details() { return mProjectDetails; }

  void updateProjectName(std::string name)
  {
    mProjectDetails.supportDirName(name);
  }

  void updateProjectPath(Path path) { mProjectDetails.parentDirectory(path); }

  ProjectMetadata const &metadata() { return mMetadata; }

private:
  ProjectMetadata mMetadata;
  ProjectDetails  mProjectDetails;
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