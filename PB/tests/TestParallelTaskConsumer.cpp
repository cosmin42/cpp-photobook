#include <gtest/gtest.h>

#include <Windows.h>

#include <PB/util/ParallelTaskConsumer.h>

TEST(TestParallelTaskConsumer, BasicTest)
{
  PB::ParallelTaskConsumer consumer;
  bool                     isFinished = consumer.finished();
  EXPECT_TRUE(isFinished);

  consumer.abort();
  isFinished = consumer.finished();
  EXPECT_TRUE(isFinished);

  std::shared_ptr<int> objectToBeChaged0 = std::make_shared<int>();
  *objectToBeChaged0 = 0;

  std::shared_ptr<int> objectToBeChaged1 = std::make_shared<int>();
  *objectToBeChaged1 = 0;

  std::shared_ptr<int> objectToBeChaged2 = std::make_shared<int>();
  *objectToBeChaged2 = 0;

  consumer.enqueue([objectToBeChaged{objectToBeChaged0}]() {
    *objectToBeChaged = 1;
    Sleep(1000);
  });
  isFinished = consumer.finished();
  EXPECT_TRUE(!isFinished);

  Sleep(1010);
  isFinished = consumer.finished();
  EXPECT_TRUE(isFinished);

  consumer.enqueue([objectToBeChaged{objectToBeChaged1}]() {
    *objectToBeChaged = 2;
    Sleep(1000);
  });

  consumer.enqueue([objectToBeChaged{objectToBeChaged2}]() {
    *objectToBeChaged = 3;
    Sleep(1000);
  });
  isFinished = consumer.finished();
  EXPECT_TRUE(!isFinished);

  Sleep(1010);
  isFinished = consumer.finished();
  EXPECT_TRUE(isFinished);

  EXPECT_EQ(*objectToBeChaged0, 1);
  EXPECT_EQ(*objectToBeChaged1, 2);
  EXPECT_EQ(*objectToBeChaged2, 3);
}