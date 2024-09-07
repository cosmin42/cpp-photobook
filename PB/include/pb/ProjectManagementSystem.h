#pragma once

#include <boost/bimap/bimap.hpp>
#include <boost/uuid/uuid.hpp>

#include <pb/DatabaseService.h>
#include <pb/project/Project.h>
#include <pb/project/ProjectMetadata.h>
#include <pb/util/Traits.h>

namespace PB {

typedef std::pair<boost::uuids::uuid, Project> IdentifyableProject;

class ProjectManagementSystemListener {
public:
  virtual ~ProjectManagementSystemListener() = default;

  virtual void onProjectMetadataRecalled() = 0;
};

class ProjectManagementSystem final {
public:
  ~ProjectManagementSystem() = default;

  void configureProjectManagementSystemListener(
      ProjectManagementSystemListener *listener);

  void
  configureDatabaseService(std::shared_ptr<DatabaseService> databaseService);
  void configurePlatformInfo(std::shared_ptr<PlatformInfo> platformInfo);

  void recallMetadata();

  std::string newAlbumName();

  boost::bimaps::bimap<boost::uuids::uuid, std::string> metadata() const;

  std::shared_ptr<IdentifyableProject> maybeLoadedProjectInfo() const;

  void deleteProject(std::string id);

private:
  static std::vector<std::string> HAPPY_WORDS;

  ProjectManagementSystemListener *mListener = nullptr;

  std::shared_ptr<PlatformInfo> mPlatformInfo = nullptr;

  boost::bimaps::bimap<boost::uuids::uuid, std::string> mProjectsMetadata;

  std::shared_ptr<DatabaseService> mDatabaseService = nullptr;

  std::shared_ptr<IdentifyableProject> maybeLoadedProject = nullptr;

  bool hasProjectName(std::string name) const;
};
} // namespace PB
