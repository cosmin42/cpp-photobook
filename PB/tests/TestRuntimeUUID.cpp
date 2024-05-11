#include <gtest/gtest.h>

#include <boost/uuid/uuid_io.hpp>

#include <pb/RuntimeUUID.h>

using namespace PB;

TEST(TestRuntimeUUID, TestNewUUID)
{
  auto firstUUID = RuntimeUUID::newUUID();
  ASSERT_TRUE(boost::uuids::to_string(firstUUID) ==
              "00000000-0000-0000-0000-000000000000");

  auto secondUUID = RuntimeUUID::newUUID();
  auto str = boost::uuids::to_string(secondUUID);
  ASSERT_TRUE(boost::uuids::to_string(secondUUID) ==
              "00000000-0000-0000-0000-000000000001");
}