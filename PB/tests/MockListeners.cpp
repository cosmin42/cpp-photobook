#include "MockListeners.h"

void clearProjectCache()
{
  std::regex uuidRegex = std::regex(
      "[0-9a-f]{8}-[0-9a-f]{4}-[0-9a-f]{4}-[0-9a-f]{4}-[0-9a-f]{12}");

  std::filesystem::remove("database.db");

  for (const auto &entry : std::filesystem::directory_iterator(".")) {
    if (std::regex_match(entry.path().stem().string(), uuidRegex)) {
      std::filesystem::remove(entry.path());
    }
  }
}

std::shared_ptr<PB::PlatformInfo> mockPlatformInfo()
{
  std::shared_ptr<PB::PlatformInfo> platform =
      std::make_shared<PB::PlatformInfo>();
  platform->installationPath = std::filesystem::current_path();
  platform->localStatePath = std::filesystem::current_path();
  platform->screenSize = std::make_pair(1920, 1080);
  return platform;
}

std::shared_ptr<PB::PlatformInfo> mockPlatformInfo(Path installationPath,
                                                   Path locaStatePath)
{
  std::shared_ptr<PB::PlatformInfo> platform =
      std::make_shared<PB::PlatformInfo>();
  platform->installationPath = installationPath;
  platform->localStatePath = locaStatePath;
  platform->screenSize = std::make_pair(1920, 1080);
  return platform;
}

std::shared_ptr<DatabaseService>
mockDatabaseService(std::shared_ptr<PB::PlatformInfo> platformInfo)
{
  auto dbService = std::make_shared<DatabaseService>();
  dbService->configurePlatformInfo(platformInfo);
  dbService->connect();
  dbService->maybeCreateTables();
  return dbService;
}

std::shared_ptr<ProjectSerializerService>
mockProjectSerializerService(std::shared_ptr<PB::PlatformInfo> platformInfo)
{
  auto projectSerializerService = std::make_shared<ProjectSerializerService>();
  projectSerializerService->configurePlatformInfo(platformInfo);
  return projectSerializerService;
}

std::shared_ptr<DurableHashService>
mockDurableHashService(std::shared_ptr<DatabaseService> databaseService)
{
  auto durableHashService = std::make_shared<DurableHashService>();
  durableHashService->configureDatabaseService(databaseService);
  return durableHashService;
}
