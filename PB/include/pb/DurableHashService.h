#pragma once

#include <pb/DatabaseService.h>
#include <pb/Platform.h>
#include <pb/util/Traits.h>

DECLARE_STRONG_UUID(ProjectId)

namespace PB {
class DurableHashService final {
public:
  ~DurableHashService() = default;

  void
  configureDatabaseService(std::shared_ptr<DatabaseService> databaseService);

  void configurePlatformInfo(std::shared_ptr<PlatformInfo> platformInfo);

  void initialize();

  bool containsHash(std::string key);

  std::string getHash(PBDev::ProjectId projectId, Path path);

  boost::bimaps::bimap<Path, std::string> hashSet(PBDev::ProjectId projectId);

  Path assemblePath(PBDev::ProjectId id, std::string hash);

private:
  static std::string computeHash(std::string key);

  std::shared_ptr<DatabaseService> mDatabaseService = nullptr;

  std::shared_ptr<PlatformInfo> mPlatformInfo = nullptr;

  std::string saltHash(std::string hash);
};
} // namespace PB
