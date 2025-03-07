#include <gtest/gtest.h>

#include "MockListeners.h"

#include <pb/export/ExportService.h>

using ::testing::_;
using ::testing::AtLeast;
using namespace PB;

TEST(TestExportService, Test0)
{
  ProgressServiceListenerTest *progressServiceListener =
      new ProgressServiceListenerTest();

  TestExportServiceListener *testExportListener =
      new TestExportServiceListener();

  ThreadSchedulerMock *threadScheduler =
      new ThreadSchedulerMock(std::chrono::milliseconds(3000));

  std::shared_ptr<ProgressService> progressService =
      std::make_shared<ProgressService>();
  progressService->configure(progressServiceListener);
  progressService->configureScheduler(threadScheduler);

  std::shared_ptr<TaskCruncher> taskCruncher = std::make_shared<TaskCruncher>();
  taskCruncher->configureProgressService(progressService);

  taskCruncher->registerPTC("export-logic", 1);
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

  std::shared_ptr<PB::IdentifyableProject> identifyableProject =
      std::make_shared<PB::IdentifyableProject>(
          std::make_pair(projectId, project));

  std::shared_ptr<ExportService> mExportService =
      std::make_shared<ExportService>();

  mExportService->configureProject(identifyableProject);
  mExportService->configurePlatformInfo(platformInfo);
  mExportService->configureTaskCruncher(taskCruncher);

  mExportService->configureExportListener(testExportListener);

  EXPECT_CALL(*testExportListener, onExportComplete(_)).Times(AtLeast(1));
  EXPECT_CALL(*progressServiceListener, progressUpdate(_)).Times(AtLeast(1));

  mExportService->exportPDFAlbum("pdfdocument",
                                 std::filesystem::current_path());

  threadScheduler->mainloop();

  if (std::filesystem::exists(std::filesystem::current_path() /
                              "pdfdocument.pdf")) {
    std::filesystem::remove(std::filesystem::current_path() /
                            "pdfdocument.pdf");
  }

  delete progressServiceListener;
  delete testExportListener;
  delete threadScheduler;
}
