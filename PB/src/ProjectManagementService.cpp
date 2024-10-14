#include <pb/services/ProjectManagementService.h>

#include <pb/Config.h>

namespace PB::Service {
#ifdef SIMULATE_FEW_HAPPY_WORDS
std::vector<std::string> ProjectManagementService::HAPPY_WORDS = {"Joyful",
                                                                 "Blissful"};
#else
std::vector<std::string> ProjectManagementService::HAPPY_WORDS = {
    "Joyful",       "Blissful",   "Radiant",       "Cheerful",
    "Exuberant",    "Jubilant",   "Gleeful",       "Delighted",
    "Content",      "Ecstatic",   "Merry",         "Jovial",
    "Upbeat",       "Elated",     "Happy",         "Sunny",
    "Lighthearted", "Buoyant",    "Festive",       "Vibrant",
    "Grateful",     "Satisfied",  "Pleased",       "Overjoyed",
    "Optimistic",   "Pleased",    "Thrilled",      "Euphoric",
    "Playful",      "Carefree",   "Radiant",       "Wonderful",
    "Jolly",        "Zestful",    "Bubbly",        "Heartwarming",
    "Cheery",       "Upbeat",     "Chirpy",        "Sanguine",
    "Enthusiastic", "Serene",     "Up-lifting",    "Giddy",
    "Gleaming",     "Bountiful",  "Exultant",      "Merry-making",
    "Comical",      "Hilarious",  "Dynamic",       "Sweet",
    "Whimsical",    "Spirited",   "Sprightly",     "Delightful",
    "Radiant",      "Uplifting",  "Optimistic",    "Tickled",
    "Chipper",      "Pleasant",   "Buoyant",       "Lively",
    "Sparkling",    "Tickled",    "Exhilarating",  "Bouncy",
    "Festal",       "Vivacious",  "Gladsome",      "Pleased",
    "Elated",       "Blithesome", "Merry-hearted", "Gleamy",
    "Zippy",        "Snappy",     "Piquant",       "Buoyant",
    "Enlivened",    "Beaming",    "Radiant",       "Jocund",
    "Grinning",     "Giggly",     "Sunny",         "Uplifted",
    "Hopeful",      "Amused",     "Merry-making",  "Overjoyed",
    "Glad",         "Contented",  "Radiant",       "Effervescent",
    "Spry",         "Snug",       "Jovial",        "Untroubled"};
#endif

std::string ProjectManagementService::newAlbumName()
{
  auto size = HAPPY_WORDS.size();
  std::srand(static_cast<unsigned int>(std::time(0)));
  int         randomIndex = std::rand() % size;
  std::string prefix = HAPPY_WORDS.at(randomIndex);
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
  mListener->onProjectMetadataRecalled();
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

void ProjectManagementService::deleteProject(std::string id) {}

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

  maybeLoadedProject = std::make_shared<IdentifyableProject>(
      std::make_pair(newProjectId, project));

  mProjectsMetadata.insert({newProjectId, project.name});
}

void ProjectManagementService::loadProject(boost::uuids::uuid id)
{
  PBDev::basicAssert(maybeLoadedProject == nullptr);
  std::string projectName = mProjectsMetadata.left.at(id);
  auto        projectPath = mPlatformInfo->projectPath(projectName);
  auto project = mProjectSerializerService->deserializeProjectInfo(projectPath);

  maybeLoadedProject =
      std::make_shared<IdentifyableProject>(std::make_pair(id, project));
}

void ProjectManagementService::unloadProject()
{
  PBDev::basicAssert(maybeLoadedProject != nullptr);
  PBDev::basicAssert(maybeLoadedProject.use_count() == 1);

  maybeLoadedProject.reset();
  maybeLoadedProject = nullptr;
}

void ProjectManagementService::saveMetadata()
{
  PBDev::basicAssert(maybeLoadedProject != nullptr);

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
  }
}

} // namespace PB
