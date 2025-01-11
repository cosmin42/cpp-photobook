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

std::shared_ptr<IdentifyableProject>
ProjectManagementService::maybeLoadedProjectInfo() const
{
  return maybeLoadedProject;
}

bool ProjectManagementService::hasProjectName(std::string name) const
{
  return mProjectsMetadata.right.find(name) != mProjectsMetadata.right.end();
}

void ProjectManagementService::deleteProject(boost::uuids::uuid id)
{
  auto projectName = mProjectsMetadata.left.at(id);

  auto projectPath = mPlatformInfo->projectPath(projectName);

  auto projectData =
      mPlatformInfo->projectFolderPath() / boost::uuids::to_string(id);

  // TODO: Check return values
  std::filesystem::remove_all(projectPath);
  std::filesystem::remove_all(projectData);

  mDatabaseService->deleteData(OneConfig::DATABASE_PROJECT_METADATA_TABLE,
                               "uuid='" + boost::uuids::to_string(id) + "'");
  mProjectsMetadata.right.erase(projectName);

  mListener->onProjectMetadataRecalled("");
}

void ProjectManagementService::deleteProjectByName(std::string name)
{
  auto projectId = mProjectsMetadata.right.at(name);

  auto projectData =
      mPlatformInfo->projectFolderPath() / boost::uuids::to_string(projectId);

  auto projectPath = mPlatformInfo->projectPath(name);

  // TODO: Check return values
  std::filesystem::remove_all(projectPath);
  std::filesystem::remove_all(projectData);

  mDatabaseService->deleteData(OneConfig::DATABASE_PROJECT_METADATA_TABLE,
                               "uuid='" + boost::uuids::to_string(projectId) +
                                   "'");
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

  mProjectSerializerService->createProjectFolderStructure(newProjectId);

  Project project;
  project.name = newAlbumName();
  project.paperSettings = paperSettings;

  Noir::inst().getLogger()->info("New project {}, {}, {}", project.name,
                                 boost::uuids::to_string(newProjectId),
                                 std::string(paperSettings));

  maybeLoadedProject = std::make_shared<IdentifyableProject>(
      std::make_pair(newProjectId, project));

  GenericImage::configureProjectPath(
      mPlatformInfo->projectSupportFolder(newProjectId) / "thumbnail-images");

  saveMetadata();
  mProjectSerializerService->saveProject(maybeLoadedProject->second);

  mProjectsMetadata.insert({newProjectId, project.name});

  preprocessDefaultWaitingImage();

  mListener->onProjectMetadataRecalled(project.name);
}

void ProjectManagementService::preprocessDefaultWaitingImage()
{
  PBDev::basicAssert(maybeLoadedProjectInfo() != nullptr);
  auto waitImagePath = mPlatformInfo->waitImage();

  auto hash = ThumbnailsTask::createThumbnailsByPath(
      waitImagePath, mPlatformInfo, maybeLoadedProjectInfo(), "wait");

  Noir::inst().getLogger()->info("Wait image processed to {}", std::get<0>(mPlatformInfo->waitThumbnails()).string());

  UNUSED(hash);
}

void ProjectManagementService::loadProject(boost::uuids::uuid id)
{
  std::string projectName = mProjectsMetadata.left.at(id);
  auto        projectPath = mPlatformInfo->projectPath(projectName);
  auto project = mProjectSerializerService->deserializeProjectInfo(projectPath);

  maybeLoadedProject =
      std::make_shared<IdentifyableProject>(std::make_pair(id, project));

  GenericImage::configureProjectPath(mPlatformInfo->projectSupportFolder(id) /
                                     "thumbnail-images");

  Noir::inst().getLogger()->info(
      "Project loaded by id: {}, {}, {}", projectName,
      boost::uuids::to_string(id),
      std::string(maybeLoadedProject->second.paperSettings));
}

void ProjectManagementService::loadProjectByName(std::string name)
{
  boost::uuids::uuid id = mProjectsMetadata.right.at(name);
  auto               projectPath = mPlatformInfo->projectPath(name);
  auto project = mProjectSerializerService->deserializeProjectInfo(projectPath);

  maybeLoadedProject =
      std::make_shared<IdentifyableProject>(std::make_pair(id, project));

  GenericImage::configureProjectPath(mPlatformInfo->projectSupportFolder(id) /
                                     "thumbnail-images");

  Noir::inst().getLogger()->info(
      "Project loaded by name: {}, {}, {}", name, boost::uuids::to_string(id),
      std::string(maybeLoadedProject->second.paperSettings));
}

void ProjectManagementService::unloadProject()
{
  PBDev::basicAssert(maybeLoadedProject != nullptr);
  maybeLoadedProject = nullptr;
  GenericImage::configureProjectPath("");
  Noir::inst().getLogger()->info("Project unloaded");
}

void ProjectManagementService::saveMetadata()
{
  auto projectId = maybeLoadedProject->first;
  auto project = maybeLoadedProject->second;

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
      if (maybeLoadedProject->second.name == oldName) {
        auto project = maybeLoadedProject->second;
        project.name = newName;
        maybeLoadedProject->second = project;
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
  mProjectSerializerService->saveProject(maybeLoadedProject->second);
}

} // namespace PB::Service
