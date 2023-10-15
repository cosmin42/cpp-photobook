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
  std::string        supportDirName;
  Path               parentDirectory;
  Path supportFolder() const { return parentDirectory / supportDirName; }
  Path projectFile() const
  {
    return parentDirectory / (supportDirName + Context::BOOK_EXTENSION);
  }

  operator std::unordered_map<std::string, std::string>() const
  {
    std::unordered_map<std::string, std::string> result;

    result["project-uuid"] = boost::uuids::to_string(uuid);
    result["project-name"] = supportDirName;
    result["project-path"] = parentDirectory.string();

    return result;
  }
};

std::variant<ProjectDetails, Error>
convert(std::unordered_map<std::string, std::string> const &map);

class Project final {
public:
  static std::optional<std::string> excludeExtension(std::string fileName)
  {
    if (fileName.find(Context::BOOK_EXTENSION) != std::string::npos) {
      return fileName.substr(
          0, fileName.length() - std::string(Context::BOOK_EXTENSION).length());
    }
    return std::nullopt;
  }

  Project() = default;

  Project(Path centralPersistencePath)
  {
    mProjectDetails.uuid = boost::uuids::random_generator()();
    mProjectDetails.supportDirName =
        boost::uuids::to_string(mProjectDetails.uuid);
    mProjectDetails.parentDirectory = centralPersistencePath;
  }

  explicit Project(ProjectDetails const &projectDetails)
      : mProjectDetails(projectDetails)
  {
  }

  ~Project() = default;

  ProjectDetails const &details() const { return mProjectDetails; }

  void updateProjectName(std::string name)
  {
    mProjectDetails.supportDirName = name;
  }

  void updateProjectPath(Path path) { mProjectDetails.parentDirectory = path; }

  std::pair<std::string, std::string> locationData()
  {
    auto uuidStr = boost::uuids::to_string(mProjectDetails.uuid);
    auto pathStr = mProjectDetails.parentDirectory.string();

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
    mSet[newProject.details().uuid] = newProject;
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