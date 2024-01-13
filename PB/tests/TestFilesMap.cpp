#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <future>

#include <pb/util/Util.h>

#include <pb/tasks/FileMapper.h>

#include <Windows.h>

using namespace PB;

using ::testing::_;

class TestFileMapSequentialTaskConsumerListener
    : public PBDev::SequentialTaskConsumerListener<MediaMapper> {
public:
  ~TestFileMapSequentialTaskConsumerListener() = default;
  MOCK_METHOD(void, started, (PB::MediaMapper const &), (override));
  MOCK_METHOD(void, finished, (PB::MediaMapper const &), (override));
  MOCK_METHOD(void, aborted, (PB::MediaMapper const &), (override));
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

  EXPECT_CALL(fileMapSequentialTaskConsumerListener, started(_));
  EXPECT_CALL(fileMapSequentialTaskConsumerListener, finished(_));
  consumer.start();
}
