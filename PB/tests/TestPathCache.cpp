#include <gtest/gtest.h>

#include "MockListeners.h"

#include <pb/Config.h>
#include <pb/project/Project.h>

TEST(TestPathCache, Test0)
{
  PB::PathCache pathCache;

  Path path = "a/b/c";

  pathCache.newHash(path);

  auto success = pathCache.contains(path);

  ASSERT_TRUE(success);

  auto hash = pathCache.hash(path);

  ASSERT_FALSE(hash.empty());
}