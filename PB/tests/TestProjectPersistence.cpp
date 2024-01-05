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

  Persistence persistence(".", &projectListener, &metadataListener);

  ASSERT_TRUE(std::filesystem::exists("database.db"));

  PB::Project project(Path("."));

  auto maybeError = Persistence::createSupportDirectory("th");

  ASSERT_FALSE(maybeError.has_value());

  Path dataPath = Path("../test-data");

  project.active().pathCache().newHash(dataPath);

  bool valid = PB::PathCache::valid(
      dataPath, project.active().pathCache().hash(dataPath));

  ASSERT_TRUE(valid);
}
