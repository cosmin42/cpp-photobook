#include <pb/DurableHashService.h>

#include <boost/uuid/uuid_io.hpp>

#include <pb/Config.h>

namespace PB {
std::string DurableHashService::computeHash(std::string key)
{
  return std::to_string(std::hash<std::string>{}(key));
}

void DurableHashService::configureDatabaseService(
    std::shared_ptr<DatabaseService> databaseService)
{
  mDatabaseService = databaseService;
}

bool DurableHashService::containsHash(std::string key)
{
  std::string predicate = "cache_path='" + key + "'";
  auto        hashFound =
      mDatabaseService->selectData(OneConfig::DATABASE_CACHE_TABLE, predicate,
                                   OneConfig::DATABASE_CACHE_HEADER.size());
  return !hashFound.empty();
}

std::string DurableHashService::getHash(PBDev::ProjectId projectId, Path path)
{
  auto projectIdStr = boost::uuids::to_string(*projectId);

  // TODO: Change this to select also by project id
  auto rawData = mDatabaseService->selectData(
      OneConfig::DATABASE_CACHE_TABLE, "path='" + path.string() + "'",
      OneConfig::DATABASE_CACHE_HEADER.size());
  auto cacheEntry = DatabaseService::deserializeCacheEntry(rawData);

  if (cacheEntry.empty()) {
    auto hash = computeHash(path.string());
    hash = saltHash(hash);

    mDatabaseService->insert<3>(
        OneConfig::DATABASE_CACHE_TABLE, OneConfig::DATABASE_CACHE_HEADER,
        {projectIdStr.c_str(), path.string().c_str(), hash.c_str()});
    return hash;
  }

  PBDev::basicAssert(cacheEntry.left.find(path) != cacheEntry.left.end());

  return cacheEntry.left.at(path);
}

std::string DurableHashService::saltHash(std::string hash)
{
  for (int i = 0; i < OneConfig::MAX_HASH_CONFLICTS; ++i) {
    std::string hashAttempt = hash + std::to_string(i);
    if (containsHash(hashAttempt)) {
      return hashAttempt;
    }
  }
  PBDev::basicAssert(false);
  return "";
}
} // namespace PB
