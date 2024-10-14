#include <gtest/gtest.h>

#include "MockListeners.h"

#include <pb/components/TSQueue.h>
#include <pb/services/LutService.h>

using namespace PB;

class ThreadSchedulerMock final : public PBDev::ThreadScheduler {
public:
  ~ThreadSchedulerMock() = default;

  void post(std::function<void()> f) override { f(); }

  void mainloop()
  {
    while (true) {
      std::function<void()> f;
      auto                  f = mQueue.dequeue(std::chrono::milliseconds(2000));
      if (f) {
        f();
      }
      else {
        throw;
      }
    }
  }

private:
  TSQueue<std::function<void()>> mQueue;
};

TEST(TestLutService, TestEmpty)
{
  ProgressServiceListenerTest *progressServiceListener =
      new ProgressServiceListenerTest();

  TestLutServiceListener *testLutServiceListener = new TestLutServiceListener();

  ThreadSchedulerMock *threadScheduler = new ThreadSchedulerMock();

  std::shared_ptr<ProgressService> progressService =
      std::make_shared<ProgressService>();
  progressService->configure(progressServiceListener);
  progressService->configureScheduler(threadScheduler);

  std::shared_ptr<TaskCruncher> taskCruncher = std::make_shared<TaskCruncher>();
  taskCruncher->configureProgressService(progressService);

  auto platformInfo = mockPlatformInfo();

  std::shared_ptr<OGLEngine> mOGLEngine = std::make_shared<OGLEngine>();
  mOGLEngine->configurePlatformInfo(platformInfo);

  std::shared_ptr<LutService> mLutService = std::make_shared<LutService>();
  mLutService->configurePlatformInfo(platformInfo);
  mLutService->configureTaskCruncher(taskCruncher);
  mLutService->configureOGLEngine(mOGLEngine);
  mLutService->configureThreadScheduler(threadScheduler);

  std::stop_source stopSource;

  mOGLEngine->start(stopSource.get_token());
  mLutService->startLutService();

  delete threadScheduler;
  delete testLutServiceListener;
  delete progressServiceListener;
}