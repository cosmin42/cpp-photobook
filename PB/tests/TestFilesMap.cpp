#include <gtest/gtest.h>

#include <Error.h>

#include <FileMapper.h>

using namespace PB;

TEST(TestFilesMap, TestConstructor)
{
  testing::internal::CaptureStdout();
  FilesMap    fileMapper(".");
  std::string output = testing::internal::GetCapturedStdout();
#ifdef WIN32
  ASSERT_EQ(output, "[Debug] FilesMap ctr .\n");
#else
  ASSERT_EQ(output, "[Debug] FilesMap ctr \x10\n");
#endif
}

TEST(TestFilesMap, TestSimpleCollection)
{
  FilesMap fileMapper("../../tests-data/Collection");

  auto map = fileMapper.map();
}
