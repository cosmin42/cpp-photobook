#include <gtest/gtest.h>

#include <pb/services/ProjectSerializerService.h>

#include "MockListeners.h"

using namespace PB;

TEST(TestProjectSerializerService, TestFolderStructure)
{
  auto                     platformInfo = mockPlatformInfo();
  ProjectSerializerService projectSerializerService;
  projectSerializerService.configurePlatformInfo(platformInfo);

  boost::uuids::uuid projectId = boost::uuids::random_generator()();

  projectSerializerService.createProjectFolderStructure(projectId);

  auto projectPath = platformInfo->localStatePath / "projects" /
                     boost::uuids::to_string(projectId);

  auto thumbnailsPath = projectPath / "thumbnail-images";

  auto deletedCount =
      std::filesystem::remove_all(platformInfo->localStatePath / "projects");

  ASSERT_TRUE(deletedCount > 0);
}

std::string readFile(Path path)
{
  std::ifstream file(path);
  std::string   content((std::istreambuf_iterator<char>(file)),
                        std::istreambuf_iterator<char>());
  return content;
}

TEST(TestProjectSerializerService, TestSaveEmptyProject)
{
  auto                     platformInfo = mockPlatformInfo();
  ProjectSerializerService projectSerializerService;
  projectSerializerService.configurePlatformInfo(platformInfo);

  boost::uuids::uuid projectId = boost::uuids::random_generator()();

  projectSerializerService.createProjectFolderStructure(projectId);

  projectSerializerService.saveProject(
      "TestName", Project(), GenericImagePtrMatrix(), GenericImagePtrLine(),
      std::vector<Path>());
  auto projectFileCreated = std::filesystem::exists(
      platformInfo->localStatePath / "projects" / "TestName.photobook");
  ASSERT_TRUE(projectFileCreated);

  std::string reference = R"({
  "root": {
    "paper-settings": {
      "height": 2480,
      "ppi": 300,
      "type": 1,
      "width": 3508
    }
  },
  "row-paths": null,
  "staged": null,
  "unstaged": null
})";

  auto projectContent = readFile(platformInfo->localStatePath / "projects" /
                                 "TestName.photobook");
  EXPECT_EQ(reference, projectContent);

  auto deletedCount =
      std::filesystem::remove_all(platformInfo->localStatePath / "projects");

  ASSERT_TRUE(deletedCount > 0);
}