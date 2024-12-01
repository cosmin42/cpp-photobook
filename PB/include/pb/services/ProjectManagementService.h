#pragma once

#include <boost/bimap/bimap.hpp>
#include <boost/uuid/uuid.hpp>

#include <pb/entities/PaperSettings.h>
#include <pb/entities/ProjectMetadata.h>
#include <pb/project/Project.h>
#include <pb/services/DatabaseService.h>
#include <pb/services/ProjectSerializerService.h>
#include <pb/util/Traits.h>

namespace PB::Service {

typedef std::pair<boost::uuids::uuid, Project> IdentifyableProject;

class ProjectManagementServiceListener {
public:
  virtual ~ProjectManagementServiceListener() = default;

  virtual void onProjectMetadataRecalled() = 0;
  virtual void onProjectRecalled() = 0;
};

class ProjectManagementService final {
public:
  ~ProjectManagementService() = default;

  void configureProjectManagementServiceListener(
      ProjectManagementServiceListener *listener);

  void
  configureDatabaseService(std::shared_ptr<DatabaseService> databaseService);
  void configurePlatformInfo(std::shared_ptr<PlatformInfo> platformInfo);
  void configureProjectSerializerService(
      std::shared_ptr<ProjectSerializerService> projectSerializerService);

  void recallMetadata();

  std::string newAlbumName();

  void newProject(PaperSettings paperSettings);
  void loadProject(boost::uuids::uuid id);
  void unloadProject();

  void saveMetadata();
  void renameProject(std::string oldName, std::string newName);
  void deleteProject(boost::uuids::uuid id);

  std::vector<std::tuple<boost::uuids::uuid, std::string, Path>>
  projectsList() const;

  boost::bimaps::bimap<boost::uuids::uuid, std::string> metadata() const;

  std::shared_ptr<IdentifyableProject> maybeLoadedProjectInfo() const;

private:
  static std::vector<std::string> HAPPY_WORDS;

  ProjectManagementServiceListener *mListener = nullptr;

  std::shared_ptr<PlatformInfo>             mPlatformInfo = nullptr;
  std::shared_ptr<DatabaseService>          mDatabaseService = nullptr;
  std::shared_ptr<ProjectSerializerService> mProjectSerializerService = nullptr;

  boost::bimaps::bimap<boost::uuids::uuid, std::string> mProjectsMetadata;

  std::shared_ptr<IdentifyableProject> maybeLoadedProject = nullptr;

  bool hasProjectName(std::string name) const;
};
} // namespace PB
