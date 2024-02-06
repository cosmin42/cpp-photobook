#include <gtest/gtest.h>

#include "MockListeners.h"

#include <pb/Config.h>
#include <pb/project/Project.h>

using namespace PB;

TEST(TestProjectPersistence, CreateNewProject)
{
  clearProjectCache();

  TestPersistenceProjectListener projectListener;

  TestPersistenceMetadataListener metadataListener;

  ASSERT_FALSE(std::filesystem::exists("database.db"));

  Persistence persistence;

  persistence.configure(Path("."));
  persistence.configure(&projectListener);
  persistence.configure(&metadataListener);

  ASSERT_TRUE(std::filesystem::exists("database.db"));

  PB::Project project;

  auto maybeError = Persistence::createSupportDirectory("th", "project-name");

  ASSERT_FALSE(maybeError.has_value());

  Path dataPath = Path("../test-data");
  /*
  project.active().pathCache.hashCreateIfMissing(dataPath);

  bool valid = PB::PathCache::valid(
      dataPath, project.active().pathCache().hash(dataPath));

  ASSERT_TRUE(valid);
  */
}
