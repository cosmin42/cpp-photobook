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

  ThreadSchedulerMock *threadScheduler =
      new ThreadSchedulerMock(std::chrono::milliseconds(5000));

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

  PB::Project project;

  std::shared_ptr<PB::IdentifyableProject> identifyableProject =
      std::make_shared<PB::IdentifyableProject>(
          std::make_pair(boost::uuids::random_generator()(), project));

  std::shared_ptr<CollageService> mCollageService =
      std::make_shared<CollageService>();

  mCollageService->configurePlatformInfo(platformInfo);
  mCollageService->configureTaskCruncher(taskCruncher);
  mCollageService->configureProject(identifyableProject);

  mCollageService->configureThumbnailsListener(
      testCollageThumbnailsMakerListener);
  mCollageService->configureCollageMakerListener(testCollageMakerListener);

  mCollageService->generateTemplatesImages();

  EXPECT_CALL(*progressServiceListener, progressUpdate(_)).Times(AtLeast(1));
  EXPECT_CALL(*testCollageThumbnailsMakerListener, onThumbnailsCreated())
      .Times(1);
  threadScheduler->mainloop();

  auto collagePaths = mCollageService->getTemplatesPaths();

  EXPECT_EQ(collagePaths.size(), 5);

  delete threadScheduler;
  delete testCollageMakerListener;
  delete testCollageThumbnailsMakerListener;
  delete progressServiceListener;
}
