#include <gtest/gtest.h>

#include "MockListeners.h"

#include <pb/services/CollageService.h>

using ::testing::_;
using ::testing::AtLeast;
using namespace PB;

TEST(TestCollageService, TestEmpty)
{
  ProgressServiceListenerTest *progressServiceListener =
      new ProgressServiceListenerTest();

  TestCollageMakerListener *testCollageMakerListener =
      new TestCollageMakerListener();

  TestCollageThumbnailsMakerListener *testCollageThumbnailsMakerListener =
      new TestCollageThumbnailsMakerListener();

  ThreadSchedulerMock *threadScheduler = new ThreadSchedulerMock();

  std::shared_ptr<ProgressService> progressService =
      std::make_shared<ProgressService>();
  progressService->configure(progressServiceListener);
  progressService->configureScheduler(threadScheduler);

  std::shared_ptr<TaskCruncher> taskCruncher = std::make_shared<TaskCruncher>();
  taskCruncher->configureProgressService(progressService);

  taskCruncher->registerPTC("search-files", 1);
  taskCruncher->registerPTC("collage-thumbnails", 1);
  taskCruncher->registerPTC("thumbnails-job", 1);
  taskCruncher->registerPTC("image-search-job", 1);
  taskCruncher->registerPTC("default", 1);

  auto platformInfo = mockPlatformInfo();

  std::shared_ptr<PB::Project> project = std::make_shared<PB::Project>();

  std::shared_ptr<CollageService> mCollageService =
      std::make_shared<CollageService>();

  mCollageService->configurePlatformInfo(platformInfo);
  mCollageService->configureTaskCruncher(taskCruncher);
  mCollageService->configureProjectId(
      boost::uuids::to_string(boost::uuids::random_generator()()));
  mCollageService->configureProject(project);

  mCollageService->configureThumbnailsListener(
      testCollageThumbnailsMakerListener);
  mCollageService->configureCollageMakerListener(testCollageMakerListener);

  mCollageService->generateTemplatesImages();

  EXPECT_CALL(*progressServiceListener, progressUpdate(_)).Times(AtLeast(1));

  threadScheduler->mainloop();

  delete threadScheduler;
  delete testCollageMakerListener;
  delete testCollageThumbnailsMakerListener;
  delete progressServiceListener;
}
