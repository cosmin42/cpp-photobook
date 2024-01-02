#include <gtest/gtest.h>

#include <pb/util/Util.h>

using namespace PB;

TEST(TestErrors, TestError)
{
  auto error = PBDev::Error() << "Test description" << PB::ErrorCode::Unknown;
  (void)error;
}
