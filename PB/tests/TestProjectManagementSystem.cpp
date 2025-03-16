#include <gtest/gtest.h>

#include <pb/services/ProjectManagementService.h>

#include "MockListeners.h"

using namespace PB;

TEST(TestProjectManagementSystem, TestRecallEmptyMetadata)
{
  auto platformInfo = mockPlatformInfo();
  auto databaseService = mockDatabaseService(platformInfo);
  TestProjectManagementServiceListener projectManagementSystemListener;

  ProjectManagementService projectManagementSystem;
  projectManagementSystem.configurePlatformInfo(platformInfo);
  projectManagementSystem.configureDatabaseService(databaseService);
  projectManagementSystem.configureProjectManagementServiceListener(
      (ProjectManagementServiceListener *)&projectManagementSystemListener);

  EXPECT_CALL(projectManagementSystemListener, onProjectMetadataRecalled(""));
  projectManagementSystem.recallMetadata();

  auto maybeIdentifyableProject =
      projectManagementSystem.maybeLoadedProjectInfo();
  ASSERT_EQ(maybeIdentifyableProject, nullptr);

  databaseService->disconnect();
  std::filesystem::remove(platformInfo->localStatePath /
                          OneConfig::DATABASE_NAME);
}

TEST(TestProjectManagementSystem, TestNewProject)
{
  auto platformInfo = mockPlatformInfo();
  auto databaseService = mockDatabaseService(platformInfo);
  auto projectSeiralizer = mockProjectSerializerService(platformInfo);
  TestProjectManagementServiceListener projectManagementSystemListener;

  ProjectManagementService projectManagementSystem;
  projectManagementSystem.configurePlatformInfo(platformInfo);
  projectManagementSystem.configureDatabaseService(databaseService);
  projectManagementSystem.configureProjectSerializerService(projectSeiralizer);
  projectManagementSystem.configureProjectManagementServiceListener(
      (ProjectManagementServiceListener *)&projectManagementSystemListener);

  EXPECT_CALL(projectManagementSystemListener, onProjectMetadataRecalled(""));
  projectManagementSystem.recallMetadata();

  auto maybeIdentifyableProject =
      projectManagementSystem.maybeLoadedProjectInfo();
  ASSERT_EQ(maybeIdentifyableProject, nullptr);

  PaperSettings paperSettings;
  projectManagementSystem.newProject(paperSettings);

  maybeIdentifyableProject = projectManagementSystem.maybeLoadedProjectInfo();
  ASSERT_NE(maybeIdentifyableProject, nullptr);

  auto metadata = projectManagementSystem.metadata();
  ASSERT_EQ(metadata.size(), 1);

  projectManagementSystem.saveMetadata();
  projectSeiralizer->saveProject(maybeIdentifyableProject->value);

  auto projectId = maybeIdentifyableProject->id;
  maybeIdentifyableProject.reset();

  projectManagementSystem.unloadProject();
  maybeIdentifyableProject = projectManagementSystem.maybeLoadedProjectInfo();
  ASSERT_EQ(maybeIdentifyableProject, nullptr);

  projectManagementSystem.loadProject(projectId);
  maybeIdentifyableProject = projectManagementSystem.maybeLoadedProjectInfo();

  ASSERT_NE(maybeIdentifyableProject, nullptr);

  databaseService->disconnect();
  std::filesystem::remove(platformInfo->localStatePath /
                          OneConfig::DATABASE_NAME);

  std::filesystem::remove_all(platformInfo->localStatePath / "projects");
}