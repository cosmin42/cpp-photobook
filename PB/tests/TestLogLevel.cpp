#include <gtest/gtest.h>

#include <PBLog.h>

using namespace PB;

TEST(TestLogLevel, TestLogLevel) {
    ASSERT_EQ(PBValue<LogLevel>::name, "LogLevel");
    ASSERT_EQ(PBValue<LogLevel>::Members<LogLevel::Warning>::name, "Warning");
}
