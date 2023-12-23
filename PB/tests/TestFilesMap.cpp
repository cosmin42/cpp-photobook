#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <pb/util/Error.h>

#include <pb/tasks/FileMapper.h>

#include <Windows.h>

using namespace PB;

class TestFileMapObserver : public PBDev::Observer {
public:
  MOCK_METHOD(void, update, (PBDev::ObservableSubject &), (override));
};

TEST(TestFilesMap, TestConstructor)
{
  PB::MediaMapper fileMapper("../test-data/file-mapping-test");
}

TEST(TestFilesMap, TestSimpleCollection)
{
  PB::MediaMapper fileMapper("../test-data/file-mapping-test");

  TestFileMapObserver observer;

  fileMapper.attach(&observer);
  EXPECT_CALL(observer, update);
  fileMapper.start();

  while (fileMapper.state() != PB::MediaMapState::Started)
    ;

  EXPECT_EQ(fileMapper.state(), PB::MediaMapState::Started);

  EXPECT_CALL(observer, update);
  while (fileMapper.state() != PB::MediaMapState::None)
    ;
  fileMapper.dettach(&observer);
}
