#include <gtest/gtest.h>

#include <Error.h>

#include <FileMapper.h>

using namespace PB;

TEST(TestFilesMap, TestConstructor)
{
  testing::internal::CaptureStdout();
  FilesMap    fileMapper(".");
  std::string output = testing::internal::GetCapturedStdout();

  ASSERT_EQ(output, "[Debug] FilesMap ctr \x10\n");
}
