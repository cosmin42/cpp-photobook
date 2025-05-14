#pragma once

#include <boost/bimap/bimap.hpp>
#include <boost/uuid/uuid.hpp>

#include <pb/components/Project.h>
#include <pb/entities/PaperSettings.h>
#include <pb/entities/ProjectMetadata.h>
#include <pb/infra/Traits.h>
#include <pb/services/DatabaseService.h>
#include <pb/services/ProjectSerializerService.h>
#include <pb/NoirMonitor.h>

namespace PB::Service {

class ProjectManagementServiceListener {
public:
  virtual ~ProjectManagementServiceListener() = default;

  virtual void onProjectMetadataRecalled(std::string focusedProjectName) = 0;
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
  void configureNoirMonitor(std::shared_ptr<NoirMonitor> noirMonitor);

  void recallMetadata();

  std::string newAlbumName();

  void newProject(PaperSettings paperSettings);
  void loadProject(std::variant<std::string, boost::uuids::uuid> nameOrId);
  void unloadProject();

  void saveMetadata();
  void renameProject(std::string oldName, std::string newName);
  void deleteProject(std::variant<std::string, boost::uuids::uuid> nameOrId);

  void save();

  std::vector<std::tuple<boost::uuids::uuid, std::string, Path>>
  projectsList() const;

  boost::bimaps::bimap<boost::uuids::uuid, std::string> metadata() const;

  IdentifiableProject maybeLoadedProjectInfo() const;

private:
  static std::vector<std::string> HAPPY_WORDS;

  ProjectManagementServiceListener *mListener = nullptr;

  std::shared_ptr<PlatformInfo>             mPlatformInfo = nullptr;
  std::shared_ptr<DatabaseService>          mDatabaseService = nullptr;
  std::shared_ptr<ProjectSerializerService> mProjectSerializerService = nullptr;
  std::shared_ptr<NoirMonitor>              mNoirMonitor = nullptr;
  boost::bimaps::bimap<boost::uuids::uuid, std::string> mProjectsMetadata;

  IdentifiableProject maybeLoadedProject = nullptr;

  bool hasProjectName(std::string name) const;
  void preprocessDefaultWaitingImage();
};
} // namespace PB::Service
