#include <gtest/gtest.h>

#include <Error.h>

using namespace PB;

TEST(TestErrors, TestError)
{
  auto error = PB::Error() << "Test description" << PB::ErrorKind::Unknown;
  (void)error;
}
