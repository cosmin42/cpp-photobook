#include <gtest/gtest.h>

#include <pb/ProjectManagementSystem.h>

#include "MockListeners.h"

using namespace PB;

TEST(TestProjectManagementSystem, TestRecallEmptyMetadata)
{
  auto platformInfo = mockPlatformInfo();
  auto databaseService = mockDatabaseService(platformInfo);
  TestProjectManagementSystemListener projectManagementSystemListener;

  ProjectManagementSystem projectManagementSystem;
  projectManagementSystem.configurePlatformInfo(platformInfo);
  projectManagementSystem.configureDatabaseService(databaseService);
  projectManagementSystem.configureProjectManagementSystemListener(
      (PB::ProjectManagementSystemListener*)&projectManagementSystemListener);

  EXPECT_CALL(projectManagementSystemListener, onProjectMetadataRecalled());
  projectManagementSystem.recallMetadata();

  auto maybeIdentifyableProject = projectManagementSystem.maybeLoadedProjectInfo();
  ASSERT_EQ(maybeIdentifyableProject, nullptr);

  databaseService->disconnect();
  std::filesystem::remove(platformInfo->localStatePath /
                          OneConfig::DATABASE_NAME);
}