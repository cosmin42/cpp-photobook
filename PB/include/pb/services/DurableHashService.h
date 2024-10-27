#pragma once

#include <pb/services/DatabaseService.h>

#include <pb/util/Traits.h>

namespace PB::Service {
class DurableHashService final {
public:
  ~DurableHashService() = default;

  void
  configureDatabaseService(std::shared_ptr<DatabaseService> databaseService);

  void createLink(std::string key, std::string value);

  bool containsHash(std::string key);

  bool containsKey(std::string key);

  std::string getHash(PBDev::ProjectId projectId, Path path);
  std::string getHash(Path path);

  void deleteHashByProjectId(PBDev::ProjectId projectId);

private:
  static PBDev::ProjectId DEFAULT_PROJECT_ID;

  static std::string computeHash(std::string key);

  std::shared_ptr<DatabaseService> mDatabaseService = nullptr;

  std::string saltHash(std::string hash);
};
} // namespace PB::Service
