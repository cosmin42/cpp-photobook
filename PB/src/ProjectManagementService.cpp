#include <pb/services/ProjectManagementService.h>

#include <pb/Config.h>
#include <pb/components/ThumbnailsTask.h>

namespace PB::Service {

std::string ProjectManagementService::newAlbumName()
{
  auto size = OneConfig::HAPPY_WORDS.size();
  std::srand(static_cast<unsigned int>(std::time(0)));
  int         randomIndex = std::rand() % size;
  std::string prefix = OneConfig::HAPPY_WORDS.at(randomIndex);
  int         index = 0;
  std::string name = prefix;

  while (hasProjectName(name)) {
    name = prefix + std::to_string(index);
    index++;
    PBDev::basicAssert(index < 1000);
  }
  return name;
}

void ProjectManagementService::configureProjectManagementServiceListener(
    ProjectManagementServiceListener *listener)
{
  mListener = listener;
}

void ProjectManagementService::configureDatabaseService(
    std::shared_ptr<DatabaseService> databaseService)
{
  mDatabaseService = databaseService;
}

void ProjectManagementService::configurePlatformInfo(
    std::shared_ptr<PlatformInfo> platformInfo)
{
  mPlatformInfo = platformInfo;
}

void ProjectManagementService::configureProjectSerializerService(
    std::shared_ptr<ProjectSerializerService> projectSerializerService)
{
  mProjectSerializerService = projectSerializerService;
}

void ProjectManagementService::configureNoirMonitor(std::shared_ptr<NoirMonitor> noirMonitor)
{
  mNoirMonitor = noirMonitor;
}

void ProjectManagementService::recallMetadata()
{
  auto metadata = mDatabaseService->selectData(
      OneConfig::DATABASE_PROJECT_METADATA_TABLE, "",
      (unsigned)OneConfig::DATABASE_PROJECT_METADATA_HEADER.size());
  mProjectsMetadata = DatabaseService::deserializeProjectMetadata(metadata);
  mListener->onProjectMetadataRecalled("");
}

boost::bimaps::bimap<boost::uuids::uuid, std::string>
ProjectManagementService::metadata() const
{
  return mProjectsMetadata;
}

IdentifiableProject ProjectManagementService::maybeLoadedProjectInfo() const
{
  return maybeLoadedProject;
}

bool ProjectManagementService::hasProjectName(std::string name) const
{
  return mProjectsMetadata.right.find(name) != mProjectsMetadata.right.end();
}

void ProjectManagementService::deleteProject(
    std::variant<std::string, boost::uuids::uuid> nameOrId)
{
  std::string id;
  std::string name;

  if (std::holds_alternative<std::string>(nameOrId)) {
    name = std::get<std::string>(nameOrId);
    id = boost::uuids::to_string(mProjectsMetadata.right.at(name));
  }
  else {
    id = boost::uuids::to_string(std::get<boost::uuids::uuid>(nameOrId));
    name = mProjectsMetadata.left.at(std::get<boost::uuids::uuid>(nameOrId));
  }

  auto projectPath = mPlatformInfo->projectPath(name);

  auto projectData = mPlatformInfo->projectFolderPath() / id;

  auto removedCount = std::filesystem::remove_all(projectPath);
  if (removedCount < 1) {
    spdlog::error("Failed to remove project path: {}", projectPath.string());
  }

  removedCount = std::filesystem::remove_all(projectData);
  if (removedCount < 1) {
    spdlog::error("Failed to remove project data path: {}",
                  projectData.string());
  }

  mDatabaseService->deleteData(OneConfig::DATABASE_PROJECT_METADATA_TABLE,
                               "uuid='" + id + "'");
  mProjectsMetadata.right.erase(name);

  mListener->onProjectMetadataRecalled("");
}

std::vector<std::tuple<boost::uuids::uuid, std::string, Path>>
ProjectManagementService::projectsList() const
{

  std::vector<std::tuple<boost::uuids::uuid, std::string, Path>> projects;
  for (auto const &it : mProjectsMetadata) {
    projects.push_back(
        {it.left, it.right, mPlatformInfo->projectPath(it.right)});
  }
  return projects;
}

void ProjectManagementService::newProject(PaperSettings paperSettings)
{
  PBDev::basicAssert(maybeLoadedProject == nullptr);

  auto newProjectId = boost::uuids::random_generator()();

  mNoirMonitor->start("New Project Creating Folder Structure", boost::uuids::to_string(newProjectId));

  mProjectSerializerService->createProjectFolderStructure(newProjectId);

  mNoirMonitor->stop("New Project Creating Folder Structure", boost::uuids::to_string(newProjectId));

  mNoirMonitor->start("New project Generating New Name", boost::uuids::to_string(newProjectId));
  Project project;
  project.name = newAlbumName();
  project.paperSettings = paperSettings;
  mNoirMonitor->stop("New project Generating New Name", boost::uuids::to_string(newProjectId));

  Noir::inst().getLogger()->info("New project {}, {}, {}", project.name,
                                 boost::uuids::to_string(newProjectId),
                                 std::string(paperSettings));

  mNoirMonitor->start("New Project Save Metadata", boost::uuids::to_string(newProjectId));

  maybeLoadedProject = makeIdentifiable(newProjectId, project);

  saveMetadata();
  
  mNoirMonitor->stop("New Project Save Metadata", boost::uuids::to_string(newProjectId));

  mNoirMonitor->start("New Project Save Project", boost::uuids::to_string(newProjectId));

  mProjectSerializerService->saveProject(maybeLoadedProject->value);

  mNoirMonitor->stop("New Project Save Project", boost::uuids::to_string(newProjectId));

  mProjectsMetadata.insert({newProjectId, project.name});

  mNoirMonitor->start("New Project Preprocess Default Waiting Image", boost::uuids::to_string(newProjectId));

  preprocessDefaultWaitingImage();

  mNoirMonitor->stop("New Project Preprocess Default Waiting Image", boost::uuids::to_string(newProjectId));

  mListener->onProjectMetadataRecalled(project.name);
}

void ProjectManagementService::preprocessDefaultWaitingImage()
{
  PBDev::basicAssert(maybeLoadedProjectInfo() != nullptr);
  auto waitImagePath = mPlatformInfo->waitImage();

  auto hash = ThumbnailsTask::createThumbnailsByPath(
      waitImagePath, mPlatformInfo, mNoirMonitor, maybeLoadedProjectInfo(), "wait");

  Noir::inst().getLogger()->info(
      "Wait image processed to {}",
      std::get<0>(mPlatformInfo->waitThumbnails()).string());

  UNUSED(hash);
}

// TODO: make this async
void ProjectManagementService::loadProject(
    std::variant<std::string, boost::uuids::uuid> nameOrId)
{
  std::string        projectName;
  boost::uuids::uuid id;
  if (std::holds_alternative<std::string>(nameOrId)) {
    projectName = std::get<std::string>(nameOrId);
    id = mProjectsMetadata.right.at(projectName);
  }
  else {
    id = std::get<boost::uuids::uuid>(nameOrId);
    projectName = mProjectsMetadata.left.at(id);
  }

  auto projectPath = mPlatformInfo->projectPath(projectName);
  auto project = mProjectSerializerService->deserializeProjectInfo(projectPath);

  maybeLoadedProject = makeIdentifiable(id, project);

  Noir::inst().getLogger()->info(
      "Project loaded by id: {}, {}, {}", projectName,
      boost::uuids::to_string(id),
      std::string(maybeLoadedProject->value.paperSettings));
}

void ProjectManagementService::unloadProject()
{
  PBDev::basicAssert(maybeLoadedProject != nullptr);
  maybeLoadedProject = nullptr;
  Noir::inst().getLogger()->info("Project unloaded");
}

void ProjectManagementService::saveMetadata()
{
  auto projectId = maybeLoadedProject->id;
  auto project = maybeLoadedProject->value;

  auto result = mDatabaseService->selectData(
      OneConfig::DATABASE_PROJECT_METADATA_TABLE,
      "uuid='" + boost::uuids::to_string(projectId) + "'",
      (unsigned)OneConfig::DATABASE_PROJECT_METADATA_HEADER.size());

  if (result.empty()) {
    mDatabaseService->insert<2>(
        OneConfig::DATABASE_PROJECT_METADATA_TABLE,
        OneConfig::DATABASE_PROJECT_METADATA_HEADER,
        {boost::uuids::to_string(projectId).c_str(), project.name.c_str()});
  }
  else {
    mDatabaseService->update<2>(
        OneConfig::DATABASE_PROJECT_METADATA_TABLE,
        OneConfig::DATABASE_PROJECT_METADATA_HEADER,
        {boost::uuids::to_string(projectId).c_str(), project.name.c_str()});
  }
}

void ProjectManagementService::renameProject(std::string oldName,
                                             std::string newName)
{
  PBDev::basicAssert(!oldName.empty());
  PBDev::basicAssert(!newName.empty());
  PBDev::basicAssert(mProjectsMetadata.right.find(oldName) !=
                     mProjectsMetadata.right.end());

  if (newName != oldName) {
    bool success = mProjectsMetadata.right.replace_key(
        mProjectsMetadata.right.find(oldName), newName);
    PBDev::basicAssert(success);

    auto &uuid = mProjectsMetadata.right.at(newName);

    mDatabaseService->update<2>(
        OneConfig::DATABASE_PROJECT_METADATA_TABLE,
        OneConfig::DATABASE_PROJECT_METADATA_HEADER,
        {boost::uuids::to_string(uuid).c_str(), newName.c_str()});

    if (maybeLoadedProject != nullptr) {
      if (maybeLoadedProject->value.name == oldName) {
        auto project = maybeLoadedProject->value;
        project.name = newName;
        maybeLoadedProject->value = project;
      }
    }

    auto newProjectPath = mPlatformInfo->projectPath(newName);
    auto oldProjectPath = mPlatformInfo->projectPath(oldName);
    std::filesystem::rename(oldProjectPath, newProjectPath);
    Noir::inst().getLogger()->info("Project renamed: {} -> {}", oldName,
                                   newName);
    mListener->onProjectMetadataRecalled("");
  }
}

void ProjectManagementService::save()
{
  mProjectSerializerService->saveProject(maybeLoadedProject->value);
}

} // namespace PB::Service
