#include "MockListeners.h"

#include <pb/services/ImportFoldersService.h>

using ::testing::_;
using ::testing::AtLeast;

using namespace PB::Service;
using namespace PB;

TEST(TestFolderImport, Test0)
{
  std::shared_ptr<ImportFoldersService> importFoldersService =
      std::make_shared<ImportFoldersService>();

  std::shared_ptr<TestImportFoldersServiceListener>
      importFoldersServiceListener =
          std::make_shared<TestImportFoldersServiceListener>();

  std::shared_ptr<ThreadSchedulerMock> threadScheduler =
      std::make_shared<ThreadSchedulerMock>(std::chrono::milliseconds(15000));

  std::shared_ptr<TaskCruncher> taskCruncher = std::make_shared<TaskCruncher>();

  taskCruncher->registerPTC("thumbnails-job", 1);
  taskCruncher->registerPTC("image-search-job", 1);
  taskCruncher->registerPTC("default", 1);

  std::filesystem::path localState =
      std::filesystem::current_path().parent_path() / "test-data" /
      "test-local-state";
  std::filesystem::path installPath =
      std::filesystem::current_path().parent_path() / "test-data" /
      "test-install-folder";

  auto platformInfo = mockPlatformInfo(installPath, localState);

  PB::Project project;

  // string to boost::uuid
  boost::uuids::string_generator gen;
  boost::uuids::uuid projectId = gen("93a24d3d-edce-48fd-a361-b63ce675039d");

  PB::IdentifiableProject identifyableProject =
      makeIdentifiable(projectId, project);

  importFoldersService->configureListener(importFoldersServiceListener.get());
  importFoldersService->configureTaskCruncher(taskCruncher);
  importFoldersService->configurePlatformInfo(platformInfo);
  importFoldersService->configureProject(identifyableProject);
  importFoldersService->addImportFolder(
      std::filesystem::current_path().parent_path().parent_path() /
      "tests-data" / "Collection" / "Maldives");

  EXPECT_CALL(*importFoldersServiceListener, onSearchingFinished(_, _))
      .Times(1);

  threadScheduler->mainloop();
}
