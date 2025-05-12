#include <pb/services/DurableHashService.h>

#include <boost/uuid/uuid_io.hpp>

#include <pb/Config.h>

namespace PB::Service {

const PBDev::ProjectId DurableCache::DEFAULT_PROJECT_ID =
    PBDev::ProjectId(RuntimeUUID::zero());

void DurableCache::configureDatabaseService(
    std::shared_ptr<DatabaseService> databaseService)
{
  mDatabaseService = databaseService;
}

void DurableCache::loadDataForProject(PBDev::ProjectId projectId)
{
  auto projectIdStr = boost::uuids::to_string(*projectId);
  auto rawData = mDatabaseService->selectData(
      OneConfig::DATABASE_CACHE_TABLE, "uuid='" + projectIdStr + "'",
      (unsigned)OneConfig::DATABASE_CACHE_HEADER.size());

  for (const auto &entry : rawData) {
    PBDev::basicAssert(entry.size() == 3);
    std::string projectId = entry[0];
    std::string key = entry[1];
    std::string value = entry[2];
    mHashCache[{projectId, key}] = value;
  }

  auto defaultIdStr = boost::uuids::to_string(*DEFAULT_PROJECT_ID);
  auto defaultRawData = mDatabaseService->selectData(
      OneConfig::DATABASE_CACHE_TABLE, "uuid='" + defaultIdStr + "'",
      (unsigned)OneConfig::DATABASE_CACHE_HEADER.size());

  for (const auto &entry : defaultRawData) {
    PBDev::basicAssert(entry.size() == 3);
    std::string projectId = entry[0];
    std::string key = entry[1];
    std::string value = entry[2];
    mHashCache[{projectId, key}] = value;
  }
}

void DurableCache::linkData(PBDev::ProjectId projectId, std::string key,
                            std::string value)
{
  auto projectIdStr = boost::uuids::to_string(*projectId);
  mHashCache[{projectIdStr, key}] = value;
  mDatabaseService->insert<3>(
      OneConfig::DATABASE_CACHE_TABLE, OneConfig::DATABASE_CACHE_HEADER,
      {projectIdStr.c_str(), key.c_str(), value.c_str()});
}

void DurableCache::linkData(std::string key, std::string value)
{
  auto idStr = boost::uuids::to_string(*DEFAULT_PROJECT_ID);
  mHashCache[{idStr, key}] = value;
  mDatabaseService->insert<3>(OneConfig::DATABASE_CACHE_TABLE,
                              OneConfig::DATABASE_CACHE_HEADER,
                              {idStr.c_str(), key.c_str(), value.c_str()});
}

void DurableCache::deleteHash(std::string key)
{
  auto idStr = boost::uuids::to_string(*DEFAULT_PROJECT_ID);
  mHashCache.erase({idStr, key});
  mDatabaseService->deleteData(OneConfig::DATABASE_CACHE_TABLE,
                               "path='" + key + "'");
}

void DurableCache::deleteByProjectId(std::string projectId)
{
  for (auto it = mHashCache.begin(); it != mHashCache.end();) {
    if (it->first.first == projectId) {
      it = mHashCache.erase(it);
    }
    else {
      ++it;
    }
  }

  mDatabaseService->deleteData(OneConfig::DATABASE_CACHE_TABLE,
                               "uuid='" + projectId + "'");
}

std::string DurableCache::createOrRetrieve(PBDev::ProjectId projectId,
                                           std::string      key)
{
  auto projectIdStr = boost::uuids::to_string(*projectId);
  if (mHashCache.find({projectIdStr, key}) != mHashCache.end()) {
    return mHashCache[{projectIdStr, key}];
  }
  auto newValue = boost::uuids::to_string(boost::uuids::random_generator()());
  linkData(projectId, key, newValue);
  return newValue;
}

std::string DurableCache::createOrRetrieve(std::string key)
{
  return createOrRetrieve(DEFAULT_PROJECT_ID, key);
}

std::optional<std::string>
DurableCache::maybeRetrieve(PBDev::ProjectId projectId, std::string key) const
{
  auto projectIdStr = boost::uuids::to_string(*projectId);
  auto it = mHashCache.find({projectIdStr, key});
  if (it != mHashCache.end()) {
    return it->second;
  }
  return std::nullopt;
}

std::optional<std::string> DurableCache::maybeRetrieve(std::string key) const
{
  return maybeRetrieve(DEFAULT_PROJECT_ID, key);
}

} // namespace PB::Service
