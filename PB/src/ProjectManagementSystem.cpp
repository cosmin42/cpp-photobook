#include <pb/ProjectManagementSystem.h>

#include <pb/Config.h>

namespace PB {
#ifdef SIMULATE_FEW_HAPPY_WORDS
std::vector<std::string> ProjectManagementSystem::HAPPY_WORDS = {"Joyful",
                                                                 "Blissful"};
#else
std::vector<std::string> ProjectManagementSystem::HAPPY_WORDS = {
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

std::string ProjectManagementSystem::newAlbumName()
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

void ProjectManagementSystem::configureProjectManagementSystemListener(
    ProjectManagementSystemListener *listener)
{
  mListener = listener;
}

void ProjectManagementSystem::configureDatabaseService(
    std::shared_ptr<DatabaseService> databaseService)
{
  mDatabaseService = databaseService;
}

void ProjectManagementSystem::configurePlatformInfo(
    std::shared_ptr<PlatformInfo> platformInfo)
{
  mPlatformInfo = platformInfo;
}

void ProjectManagementSystem::recallMetadata()
{
  auto metadata = mDatabaseService->selectData(
      OneConfig::DATABASE_PROJECT_METADATA_TABLE, "",
      OneConfig::DATABASE_PROJECT_METADATA_HEADER.size());
  mProjectsMetadata = DatabaseService::deserializeProjectMetadata(metadata);
  mListener->onProjectMetadataRecalled();
}

boost::bimaps::bimap<boost::uuids::uuid, std::string>
ProjectManagementSystem::metadata() const
{
  return mProjectsMetadata;
}

std::shared_ptr<IdentifyableProject>
ProjectManagementSystem::maybeLoadedProjectInfo() const
{
  return maybeLoadedProject;
}

bool ProjectManagementSystem::hasProjectName(std::string name) const
{
  return mProjectsMetadata.right.find(name) != mProjectsMetadata.right.end();
}

void ProjectManagementSystem::deleteProject(std::string id)
{
  mDatabaseService->deleteData(OneConfig::DATABASE_PROJECT_METADATA_TABLE,
                               "uuid = '" + id + "'");

  auto projectPath = mPlatformInfo->localStatePath / "projects" / id;
  std::filesystem::remove(projectPath);
  mDatabaseService->deleteData(OneConfig::DATABASE_PROJECT_METADATA_TABLE,
                               "uuid = '" + std::string(id) + "'");
}
} // namespace PB
