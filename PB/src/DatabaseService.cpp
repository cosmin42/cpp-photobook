#include <pb/DatabaseService.h>

#include <boost/uuid/uuid_generators.hpp>

#include <pb/Config.h>

namespace PB {

DatabaseService ::~DatabaseService() { disconnect(); }

void DatabaseService::configurePlatformInfo(
    std::shared_ptr<PlatformInfo> platform)
{
  mPlatform = platform;
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

void DatabaseService::disconnect()
{
  if (mDatabase) {
    auto rc = sqlite3_close(mDatabase);
    PBDev::basicAssert(rc == SQLITE_OK);
    mDatabase = nullptr;
  }
}

void DatabaseService::maybeCreateTables()
{
  // TODO: Centralize this somehow from the configuration
  maybeCreateTable<2>(OneConfig::DATABASE_PROJECTS_DATA);
  maybeCreateTable<3>(OneConfig::DATABASE_CACHE_DATA);
}

bool DatabaseService::checkTableExists(std::string tableName)
{
  std::string query = "SELECT name FROM sqlite_master WHERE type='table' AND "
                      "name='" +
                      tableName + "';";

  sqlite3_stmt *stmt;
  auto          success =
      sqlite3_prepare_v2(mDatabase, query.c_str(), -1, &stmt, nullptr);

  if (success != SQLITE_OK) {
    return false;
  }

  if (sqlite3_step(stmt) == SQLITE_ROW) {
    sqlite3_finalize(stmt);
    return true;
  }
  sqlite3_finalize(stmt);
  return false;
}

boost::bimaps::bimap<boost::uuids::uuid, std::string>
DatabaseService::deserializeProjectMetadata(
    std::vector<std::vector<std::string>> raw)
{
  boost::bimaps::bimap<boost::uuids::uuid, std::string> map;

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
    map.insert({uuid, path});
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
