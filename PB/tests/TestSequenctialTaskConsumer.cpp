#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <pb/Config.h>
#include <pb/tasks/SequentialTaskConsumer.h>

class TaskExample {
public:
  void taskStep()
  {
    mCounter++;
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
  }

  bool stoppingCondition() { return mCounter >= END_COUNTER; }

  int counter() const { return mCounter; }

private:
  static constexpr int END_COUNTER = 100;

  int mCounter = 0;
};

class TestSequentialTaskConsumerListener
    : public PBDev::SequentialTaskConsumerListener<TaskExample> {
public:
  MOCK_METHOD(void, started, (TaskExample const &), (override));
  MOCK_METHOD(void, finished, (TaskExample const &), (override));
  MOCK_METHOD(void, aborted, (TaskExample const &), (override));
};

TEST(TestSequenctialTaskConsumer, TestCreation)
{
  std::stop_source                           sStopSource;
  TaskExample                                taskExample;
  PBDev::SequentialTaskConsumer<TaskExample> consumer;
  TestSequentialTaskConsumerListener         listener;
  consumer.configure(taskExample);
  consumer.configure(sStopSource.get_token());
  consumer.configure(
      (PBDev::SequentialTaskConsumerListener<TaskExample> *)&listener);

  EXPECT_CALL(listener, started);
  EXPECT_CALL(listener, aborted);
  consumer.start();

  std::this_thread::sleep_for(std::chrono::milliseconds(100*20));
  sStopSource.request_stop();

  std::this_thread::sleep_for(std::chrono::milliseconds(150));

  EXPECT_TRUE(consumer.task().counter() < 30);
}
