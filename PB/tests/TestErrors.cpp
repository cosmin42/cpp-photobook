#include <gtest/gtest.h>

#include <pb/Error.h>

using namespace PB;

TEST(TestErrors, TestError)
{
  auto error = PB::Error() << "Test description" << PB::ErrorCode::Unknown;
  (void)error;
}
