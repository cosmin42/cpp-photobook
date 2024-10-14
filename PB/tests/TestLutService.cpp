#include <gtest/gtest.h>

#include "MockListeners.h"

#include <pb/services/LutService.h>
#
using namespace PB;

TEST(TestLutService, TestEmpty)
{
  // mLutService->configureThreadScheduler(threadScheduler);

  ProgressServiceListenerTest *progressServiceListener =
      new ProgressServiceListenerTest();

  TestLutServiceListener *testLutServiceListener = new TestLutServiceListener();

  std::shared_ptr<ProgressService> progressService =
      std::make_shared<ProgressService>();
  progressService->configure(progressServiceListener);

  std::shared_ptr<TaskCruncher> taskCruncher = std::make_shared<TaskCruncher>();
  taskCruncher->configureProgressService(progressService);

  auto platformInfo = mockPlatformInfo();

  std::shared_ptr<OGLEngine> mOGLEngine = std::make_shared<OGLEngine>();
  mOGLEngine->configurePlatformInfo(platformInfo);

  std::shared_ptr<LutService> mLutService = std::make_shared<LutService>();
  mLutService->configurePlatformInfo(platformInfo);
  mLutService->configureTaskCruncher(taskCruncher);
  mLutService->configureOGLEngine(mOGLEngine);

  std::stop_source stopSource;

  mOGLEngine->start(stopSource.get_token());
  mLutService->startLutService();


  delete testLutServiceListener;
  delete progressServiceListener;
}