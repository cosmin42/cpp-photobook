#include <gtest/gtest.h>

#include <pb/Config.h>
#include <pb/DurableHashService.h>

#include "MockListeners.h"

using namespace PB;

TEST(TestDurableHashService, TestInit)
{
  auto               platformInfo = mockPlatformInfo();
  auto               databaseService = mockDatabaseService(platformInfo);
  DurableHashService durableHashService;
  durableHashService.configureDatabaseService(databaseService);
  databaseService->disconnect();
  std::filesystem::remove(platformInfo->localStatePath /
                          OneConfig::DATABASE_NAME);
}

TEST(TestDurableHashService, TestContainsHash)
{
  auto platformInfo = mockPlatformInfo();
  auto databaseService = mockDatabaseService(platformInfo);

  DurableHashService durableHashService;
  durableHashService.configureDatabaseService(databaseService);

  bool contains = durableHashService.containsHash("test");

  ASSERT_FALSE(contains);

  databaseService->disconnect();
  std::filesystem::remove(platformInfo->localStatePath /
                          OneConfig::DATABASE_NAME);
}

TEST(TestDurableHashService, TestComputeHash)
{
  auto platformInfo = mockPlatformInfo();
  auto databaseService = mockDatabaseService(platformInfo);

  DurableHashService durableHashService;
  durableHashService.configureDatabaseService(databaseService);

  std::string hash = durableHashService.getHash(
      PBDev::ProjectId(boost::uuids::uuid()), "test");

  ASSERT_FALSE(hash.empty());

  bool contains = durableHashService.containsHash(hash);

  ASSERT_TRUE(contains);

  databaseService->disconnect();
  std::filesystem::remove(platformInfo->localStatePath /
                          OneConfig::DATABASE_NAME);
}

TEST(TestDurableHashService, TestDeleteHashByProjectId)
{
  auto platformInfo = mockPlatformInfo();
  auto databaseService = mockDatabaseService(platformInfo);

  DurableHashService durableHashService;
  durableHashService.configureDatabaseService(databaseService);

  std::string hash = durableHashService.getHash(
	  PBDev::ProjectId(boost::uuids::uuid()), "test");

  ASSERT_FALSE(hash.empty());

  bool contains = durableHashService.containsHash(hash);

  ASSERT_TRUE(contains);

  durableHashService.deleteHashByProjectId(
	  PBDev::ProjectId(boost::uuids::uuid()));

  contains = durableHashService.containsHash(hash);

  ASSERT_FALSE(contains);

  databaseService->disconnect();
  std::filesystem::remove(platformInfo->localStatePath /
						  OneConfig::DATABASE_NAME);
}