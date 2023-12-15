#include <gtest/gtest.h>

#include <pb/Config.h>
#include <pb/util/SequentialTaskConsumer.h>

// TODO: Make this OS independent
#include <Windows.h>

class TestSequentialTaskConsumer : public PB::SequentialTaskConsumer {
public:
  TestSequentialTaskConsumer(std::stop_token stopToken)
      : SequentialTaskConsumer(stopToken)
  {
  }

  void executeSingleTask() override
  {
    mCounter++;
    Sleep(100);
  }

  void finish() override { mFinish = true; }

  bool stoppingCondition() override { return mCounter >= END_COUNTER; }

  bool finished() const { return mFinish; }

  int counter() const { return mCounter; }

private:
  static constexpr int END_COUNTER = 100;

  int  mCounter = 0;
  bool mFinish = false;
};

TEST(TestSequenctialTaskConsumer, TestCreation)
{
  std::stop_source           sStopSource;
  TestSequentialTaskConsumer taskConsumer(sStopSource.get_token());
  taskConsumer.start();

  Sleep(100 * 20);
  sStopSource.request_stop();

  while (!taskConsumer.finished())
    ;

  EXPECT_TRUE(taskConsumer.counter() < 30);
}
