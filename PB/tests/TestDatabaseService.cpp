#include <gtest/gtest.h>

#include <queue>

#include <pb/DatabaseService.h>
#include <pb/ProgressManager.h>
#include <pb/TaskCruncher.h>
#include <pb/ThreadScheduler.h>

#include "MockListeners.h"

using namespace PB;

TEST(TestDatabaseService, TestConnect)
{
  auto            platformInfo = mockPlatformInfo();
  DatabaseService dbService;
  dbService.configurePlatformInfo(platformInfo);
  dbService.connect();
  ASSERT_TRUE(std::filesystem::exists(platformInfo->localStatePath /
                                      OneConfig::DATABASE_NAME));
  dbService.disconnect();
  std::filesystem::remove(platformInfo->localStatePath /
                          OneConfig::DATABASE_NAME);
}

TEST(TestDatabaseService, TestMaybeCreateTables)
{
  auto            platformInfo = mockPlatformInfo();
  DatabaseService dbService;
  dbService.configurePlatformInfo(platformInfo);
  dbService.connect();
  dbService.maybeCreateTables();
  ASSERT_TRUE(
      dbService.checkTableExists(OneConfig::DATABASE_PROJECT_METADATA_TABLE));
  ASSERT_TRUE(dbService.checkTableExists(OneConfig::DATABASE_CACHE_TABLE));
  dbService.disconnect();
  auto databasePath = platformInfo->localStatePath / OneConfig::DATABASE_NAME;
  std::filesystem::remove(databasePath);
}

TEST(TestDatabaseService, TestInsert)
{
  auto            platformInfo = mockPlatformInfo();
  DatabaseService dbService;
  dbService.configurePlatformInfo(platformInfo);
  dbService.connect();
  dbService.maybeCreateTables();
  ASSERT_TRUE(
      dbService.checkTableExists(OneConfig::DATABASE_PROJECT_METADATA_TABLE));
  ASSERT_TRUE(dbService.checkTableExists(OneConfig::DATABASE_CACHE_TABLE));

  std::array<const char *, 2> metadataData = {"00-00-00", "a/b/c"};

  dbService.insert<2>(OneConfig::DATABASE_PROJECT_METADATA_TABLE,
                      OneConfig::DATABASE_PROJECT_METADATA_HEADER,
                      metadataData);

  std::array<const char *, 3> cacheData = {"00-00-00", "a/b/c", "d/e/f"};
  dbService.insert<3>(OneConfig::DATABASE_CACHE_TABLE,
                      OneConfig::DATABASE_CACHE_HEADER, cacheData);

  auto result =
      dbService.selectData(OneConfig::DATABASE_PROJECT_METADATA_TABLE, "", 3);
  ASSERT_EQ(result.size(), 1);

  result = dbService.selectData(OneConfig::DATABASE_CACHE_TABLE, "", 3);
  ASSERT_EQ(result.size(), 1);

  dbService.disconnect();
  std::filesystem::remove(platformInfo->localStatePath /
                          OneConfig::DATABASE_NAME);
}

TEST(TestDatabaseService, TestDelete)
{
  auto            platformInfo = mockPlatformInfo();
  DatabaseService dbService;
  dbService.configurePlatformInfo(platformInfo);
  dbService.connect();
  dbService.maybeCreateTables();
  ASSERT_TRUE(
      dbService.checkTableExists(OneConfig::DATABASE_PROJECT_METADATA_TABLE));
  ASSERT_TRUE(dbService.checkTableExists(OneConfig::DATABASE_CACHE_TABLE));

  std::array<const char *, 2> metadataData = {"00-00-00", "a/b/c"};

  dbService.insert<2>(OneConfig::DATABASE_PROJECT_METADATA_TABLE,
                      OneConfig::DATABASE_PROJECT_METADATA_HEADER,
                      metadataData);

  std::array<const char *, 3> cacheData = {"00-00-00", "a/b/c", "d/e/f"};
  dbService.insert<3>(OneConfig::DATABASE_CACHE_TABLE,
                      OneConfig::DATABASE_CACHE_HEADER, cacheData);

  auto result =
      dbService.selectData(OneConfig::DATABASE_PROJECT_METADATA_TABLE, "", 3);
  ASSERT_EQ(result.size(), 1);

  result = dbService.selectData(OneConfig::DATABASE_CACHE_TABLE, "", 3);
  ASSERT_EQ(result.size(), 1);

  dbService.deleteData(OneConfig::DATABASE_PROJECT_METADATA_TABLE,
                       "uuid='00-00-00'");
  dbService.deleteData(OneConfig::DATABASE_CACHE_TABLE, "uuid='00-00-00'");

  result =
      dbService.selectData(OneConfig::DATABASE_PROJECT_METADATA_TABLE, "", 3);
  ASSERT_EQ(result.size(), 0);

  result = dbService.selectData(OneConfig::DATABASE_CACHE_TABLE, "", 3);
  ASSERT_EQ(result.size(), 0);

  dbService.disconnect();
  std::filesystem::remove(platformInfo->localStatePath /
                          OneConfig::DATABASE_NAME);
}