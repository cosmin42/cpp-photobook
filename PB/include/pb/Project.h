#pragma once

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

template <typename PersistenceType> class Project final {
public:
  Project()
  {
    mProjectDetails.uuid = boost::uuids::random_generator()();
    mProjectDetails.name = "Untitled";
    mProjectDetails.parentDirectory = PersistenceType::localFolder();
  }
  ~Project() = default;

  ProjectDetails &details() { return mProjectDetails; }

private:
  ProjectDetails mProjectDetails;
};

template <typename PersistenceType> class ProjectsSet {
public:
  void create()
  {
    Project<PersistenceType> newProject;
    mSet[newProject.details().uuid] = newProject;
  }

private:
  std::unordered_map<boost::uuids::uuid, Project<PersistenceType>> mSet;
};

} // namespace PB