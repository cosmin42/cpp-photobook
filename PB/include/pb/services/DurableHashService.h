#pragma once

#include <pb/services/DatabaseService.h>

#include <pb/infra/Traits.h>

namespace PB::Service {

class DurableCache final {
public:
  ~DurableCache() = default;

  void
  configureDatabaseService(std::shared_ptr<DatabaseService> databaseService);

  void loadDataForProject(PBDev::ProjectId projectId);

  void linkData(PBDev::ProjectId projectId, std::string key, std::string value);
  void linkData(std::string key, std::string value);

  void deleteHash(std::string key);
  void deleteByProjectId(std::string projectId);

  std::string createOrRetrieve(PBDev::ProjectId projectId, std::string key);
  std::string createOrRetrieve(std::string key);

  std::optional<std::string> maybeRetrieve(PBDev::ProjectId projectId,
                                           std::string      key) const;
  std::optional<std::string> maybeRetrieve(std::string key) const;

private:
  static const PBDev::ProjectId DEFAULT_PROJECT_ID;

  std::map<std::pair<std::string, std::string>, std::string>
      mHashCache; // key = (projectId, path), value = string

  std::shared_ptr<DatabaseService> mDatabaseService = nullptr;
};

} // namespace PB::Service
