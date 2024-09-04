#include <pb/DatabaseService.h>

#include <boost/uuid/uuid_generators.hpp>

#include <pb/Config.h>

namespace PB {

DatabaseService ::~DatabaseService() {}

void DatabaseService::configurePlatformInfo(
    std::shared_ptr<PlatformInfo> platform)
{
  mPlatform = platform;
}

void DatabaseService::configureThreadScheduler(
    std::shared_ptr<PBDev::ThreadScheduler> threadScheduler)
{
  mThreadScheduler = threadScheduler;
}

void DatabaseService::connect()
{
  Path databasePath = mPlatform->localStatePath / OneConfig::DATABASE_NAME;

  if (sqlite3_open(databasePath.string().c_str(), &mDatabase) != SQLITE_OK) {
    sqlite3_close(mDatabase);
    mDatabase = nullptr;
    PBDev::basicAssert(false);
  }
}

void DatabaseService::maybeCreateTables()
{
  // TODO: Centralize this somehow from the configuration
  maybeCreateTable<2>(OneConfig::DATABASE_PROJECTS_DATA);
  maybeCreateTable<3>(OneConfig::DATABASE_CACHE_DATA);
}

std::unordered_map<boost::uuids::uuid, std::string,
                   boost::hash<boost::uuids::uuid>>
DatabaseService::deserializeProjectMetadata(
    std::vector<std::vector<std::string>> raw)
{
  std::unordered_map<boost::uuids::uuid, std::string,
                     boost::hash<boost::uuids::uuid>>
      map;

  for (auto const &row : raw) {
    PBDev::basicAssert(row.size() == 3);
    boost::uuids::uuid uuid;
    std::string        path;
    for (int i = 0; i < row.size(); i++) {
      if (i == 1) {
        auto generator = boost::uuids::string_generator();
        uuid = generator(row[i]);
      }
      else if (i == 2) {
        path = row[i];
      }
    }
    map[uuid] = path;
  }

  return map;
}

boost::bimaps::bimap<Path, std::string> DatabaseService::deserializeCacheEntry(
    std::vector<std::vector<std::string>> raw)
{

  boost::bimaps::bimap<Path, std::string> map;

  for (auto const &row : raw) {
    PBDev::basicAssert(row.size() == 3);
    Path        path;
    std::string hash;
    for (int i = 0; i < row.size(); i++) {
      if (i == 1) {
        path = Path(row[i]);
      }
      else if (i == 2) {
        hash = row[i];
      }
    }
    map.insert({path, hash});
  }

  return map;
}

} // namespace PB
