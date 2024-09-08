#pragma once

#include <pb/DatabaseService.h>

#include <pb/util/Traits.h>

DECLARE_STRONG_UUID(ProjectId)

namespace PB {
class DurableHashService final {
public:
  ~DurableHashService() = default;

  void
  configureDatabaseService(std::shared_ptr<DatabaseService> databaseService);

  bool containsHash(std::string key);

  std::string getHash(PBDev::ProjectId projectId, Path path);

  void deleteHashByProjectId(PBDev::ProjectId projectId);

private:
  static std::string computeHash(std::string key);

  std::shared_ptr<DatabaseService> mDatabaseService = nullptr;

  std::string saltHash(std::string hash);
};
} // namespace PB
