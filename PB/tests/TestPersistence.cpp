#include <gtest/gtest.h>

#include <pb/persistence/Persistence.h>

using namespace PB;

TEST(TestPersistence, ReadWrite)
{
  PB::Persistence<void> persistence;

  persistence.setLocalFolder(PB::Path("."));

  persistence.cache()["test1"] = "test2";

  persistence.write([](std::optional<PB::Error> maybeError) {
    ASSERT_TRUE(!maybeError.has_value());
    PB::Persistence<void> otherPersistence;
    otherPersistence.setLocalFolder(PB::Path("."));
    otherPersistence.load([](std::optional<PB::Error> maybeError) {
      ASSERT_TRUE(!maybeError.has_value());
    });
  });
}
