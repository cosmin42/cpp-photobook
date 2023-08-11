#include <gtest/gtest.h>

#include <Log.h>
#include <StandardPrinter.h>

using namespace PB;

TEST(TestLogLevel, TestLogLevel)
{
  ASSERT_EQ(PBValue<LogLevel>::name, "LogLevel");
  ASSERT_EQ(PBValue<LogLevel>::Members<LogLevel::Warning>::name, "Warning");
}

TEST(TestLogLevel, TestPrinter)
{
  CC::StandardPrinter standardPrinter;

  standardPrinter.printDebug("Something\n");
}
