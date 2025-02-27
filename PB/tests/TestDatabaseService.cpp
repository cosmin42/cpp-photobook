#include <gtest/gtest.h>

#include <queue>

#include <pb/infra/TaskCruncher.h>
#include <pb/infra/ThreadScheduler.h>
#include <pb/services/DatabaseService.h>
#include <pb/services/ProgressService.h>

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
      dbService.selectData(OneConfig::DATABASE_PROJECT_METADATA_TABLE, "",
                           OneConfig::DATABASE_PROJECT_METADATA_HEADER.size());
  ASSERT_EQ(result.size(), 1);

  result = dbService.selectData(OneConfig::DATABASE_CACHE_TABLE, "",
                                OneConfig::DATABASE_CACHE_HEADER.size());
  ASSERT_EQ(result.size(), 1);

  dbService.disconnect();
  std::filesystem::remove(platformInfo->localStatePath /
                          OneConfig::DATABASE_NAME);
}

TEST(TestDatabaseService, TestUpdate)
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
      dbService.selectData(OneConfig::DATABASE_PROJECT_METADATA_TABLE, "",
                           OneConfig::DATABASE_PROJECT_METADATA_HEADER.size());
  ASSERT_EQ(result.size(), 1);

  result = dbService.selectData(OneConfig::DATABASE_CACHE_TABLE, "",
                                OneConfig::DATABASE_CACHE_HEADER.size());
  ASSERT_EQ(result.size(), 1);

  std::array<const char *, 2> metadataData2 = {"00-00-00", "a/b/c/d"};
  dbService.update<2>(OneConfig::DATABASE_PROJECT_METADATA_TABLE,
                      OneConfig::DATABASE_PROJECT_METADATA_HEADER,
                      metadataData2);

  std::array<const char *, 3> cacheData2 = {"00-00-00", "a/b/c", "d/e/f/g"};
  dbService.update<3>(OneConfig::DATABASE_CACHE_TABLE,
                      OneConfig::DATABASE_CACHE_HEADER, cacheData2);

  result =
      dbService.selectData(OneConfig::DATABASE_PROJECT_METADATA_TABLE, "",
                           OneConfig::DATABASE_PROJECT_METADATA_HEADER.size());
  ASSERT_EQ(result.size(), 1);
  ASSERT_STREQ(result[0][1].c_str(), "a/b/c/d");

  result = dbService.selectData(OneConfig::DATABASE_CACHE_TABLE, "",
                                OneConfig::DATABASE_CACHE_HEADER.size());
  ASSERT_EQ(result.size(), 1);
  ASSERT_STREQ(result[0][2].c_str(), "d/e/f/g");

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
      dbService.selectData(OneConfig::DATABASE_PROJECT_METADATA_TABLE, "",
                           OneConfig::DATABASE_PROJECT_METADATA_HEADER.size());
  ASSERT_EQ(result.size(), 1);

  result = dbService.selectData(OneConfig::DATABASE_CACHE_TABLE, "",
                                OneConfig::DATABASE_CACHE_HEADER.size());
  ASSERT_EQ(result.size(), 1);

  dbService.deleteData(OneConfig::DATABASE_PROJECT_METADATA_TABLE,
                       "uuid='00-00-00'");
  dbService.deleteData(OneConfig::DATABASE_CACHE_TABLE, "uuid='00-00-00'");

  result =
      dbService.selectData(OneConfig::DATABASE_PROJECT_METADATA_TABLE, "",
                           OneConfig::DATABASE_PROJECT_METADATA_HEADER.size());
  ASSERT_EQ(result.size(), 0);

  result = dbService.selectData(OneConfig::DATABASE_CACHE_TABLE, "",
                                OneConfig::DATABASE_CACHE_HEADER.size());
  ASSERT_EQ(result.size(), 0);

  dbService.disconnect();
  std::filesystem::remove(platformInfo->localStatePath /
                          OneConfig::DATABASE_NAME);
}