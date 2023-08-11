#include <gtest/gtest.h>

#include <common/Log.h>
#include <common/StandardPrinter.h>

using namespace PB;

TEST(TestLogLevel, TestLogLevel)
{
  ASSERT_EQ(PBValue<LogLevel>::name, "LogLevel");
  ASSERT_EQ(PBValue<LogLevel>::Members<LogLevel::Warning>::name, "Warning");
}

TEST(TestLogLevel, TestPrinter)
{
  PB::StandardPrinter standardPrinter;

  testing::internal::CaptureStdout();

  standardPrinter.printDebug("Something\n");

  std::string output = testing::internal::GetCapturedStdout();

  ASSERT_EQ(output, "[Debug] Something\n");
}
