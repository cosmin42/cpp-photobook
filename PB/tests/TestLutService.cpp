#include <gtest/gtest.h>

#include "MockListeners.h"

#include <pb/services/LutService.h>

using ::testing::_;
using ::testing::AtLeast;
using namespace PB;

TEST(TestLutService, TestEmpty)
{
  ProgressServiceListenerTest *progressServiceListener =
      new ProgressServiceListenerTest();

  TestLutServiceListener *testLutServiceListener = new TestLutServiceListener();

  ThreadSchedulerMock *threadScheduler =
      new ThreadSchedulerMock(std::chrono::milliseconds(3000));

  std::shared_ptr<ProgressService> progressService =
      std::make_shared<ProgressService>();
  progressService->configure(progressServiceListener);
  progressService->configureScheduler(threadScheduler);

  std::shared_ptr<TaskCruncher> taskCruncher = std::make_shared<TaskCruncher>();
  taskCruncher->configureProgressService(progressService);

  taskCruncher->registerPTC("search-files", 1);
  taskCruncher->registerPTC("lut-icons", 1);
  taskCruncher->registerPTC("thumbnails-job", 1);
  taskCruncher->registerPTC("default", 1);

  auto platformInfo = mockPlatformInfo();

  std::shared_ptr<OGLEngine> mOGLEngine = std::make_shared<OGLEngine>();
  mOGLEngine->configurePlatformInfo(platformInfo);

  std::shared_ptr<LutService> mLutService = std::make_shared<LutService>();
  mLutService->configurePlatformInfo(platformInfo);
  mLutService->configureTaskCruncher(taskCruncher);
  mLutService->configureOGLEngine(mOGLEngine);
  mLutService->configureThreadScheduler(threadScheduler);
  mLutService->configureLutServiceListener(testLutServiceListener);
  mLutService->configureDurableHashService(
      mockDurableHashService(mockDatabaseService(platformInfo)));

  EXPECT_CALL(*progressServiceListener, progressUpdate(_)).Times(AtLeast(100));
  EXPECT_CALL(*testLutServiceListener, onLutAdded(_)).Times(AtLeast(100));

  mOGLEngine->start();
  mLutService->startLutService();

  threadScheduler->mainloop();

  // count files in folder
  auto count = std::count_if(
      std::filesystem::directory_iterator(platformInfo->localStatePath /
                                          "processed-luts"),
      std::filesystem::directory_iterator(), [](auto) { return true; });

  EXPECT_EQ(count, 296);
  std::filesystem::remove_all(platformInfo->processedLutsPath());

  delete threadScheduler;
  delete testLutServiceListener;
  delete progressServiceListener;
}