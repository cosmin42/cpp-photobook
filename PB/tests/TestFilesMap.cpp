#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <future>

#include <pb/util/Util.h>

#include <pb/tasks/FileMapper.h>

using namespace PB;

using ::testing::_;

class TestFileMapSequentialTaskConsumerListener
    : public PBDev::SequentialTaskConsumerListener<MediaMapper> {
public:
  ~TestFileMapSequentialTaskConsumerListener() = default;
  MOCK_METHOD(void, STCStarted, (PB::MediaMapper const &), (override));
  MOCK_METHOD(void, STCFinished, (PB::MediaMapper const &), (override));
  MOCK_METHOD(void, STCAborted, (PB::MediaMapper const &), (override));
};

TEST(TestFilesMap, TestConstructor)
{
  PB::MediaMapper fileMapper("../test-data/file-mapping-test");
}

TEST(TestFilesMap, TestSimpleCollection)
{
  PB::MediaMapper fileMapper("../test-data/file-mapping-test");

  TestFileMapSequentialTaskConsumerListener
      fileMapSequentialTaskConsumerListener;

  PBDev::SequentialTaskConsumer<MediaMapper> consumer;
  consumer.configure((PBDev::SequentialTaskConsumerListener<MediaMapper>
                          *)&fileMapSequentialTaskConsumerListener);
  consumer.configure(fileMapper);

  EXPECT_CALL(fileMapSequentialTaskConsumerListener, STCStarted(_));
  EXPECT_CALL(fileMapSequentialTaskConsumerListener, STCFinished(_));
  consumer.start();
}
