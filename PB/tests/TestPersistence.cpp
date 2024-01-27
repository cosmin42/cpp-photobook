#include <gtest/gtest.h>

#include <pb/Config.h>
#include <pb/project/Project.h>

using namespace PB;

TEST(TestPersistence, Serialization0)
{
  std::string jsonResult =
      "{\"path-cache\":{\"a/a/c\":\"9629090436824982610\",\"a/b/"
      "c\":\"11453755272124169963\",\"a/c\":\"16582467240682114478\"}}";
  PathCache pathCache;
  pathCache.newHash(Path("a/b/c"));
  pathCache.newHash(Path("a/c"));
  pathCache.newHash(Path("a/a/c"));

  auto errorOrResult =
      PB::Text::serialize<PathCache>(0, {"cache-path", pathCache});
  ASSERT_TRUE(std::holds_alternative<Json>(errorOrResult));
  ASSERT_TRUE(Json::parse(jsonResult) == std::get<Json>(errorOrResult));
}

TEST(TestPersistence, Deserialization)
{
  std::string jsonData =
      "{\"path-cache\":{\"a/a/c\":\"9629090436824982610\",\"a/b/"
      "c\":\"11453755272124169963\",\"a/c\":\"16582467240682114478\"}}";
  Json json = Json::parse(jsonData);
  auto pathCacheOrError =
      PB::Text::deserialize<PathCache>(json, "path-cache", PathCache(), true);

  ASSERT_FALSE(std::holds_alternative<PBDev::Error>(pathCacheOrError));
  auto pathCache = std::get<PB::PathCache>(pathCacheOrError);
  ASSERT_TRUE(pathCache.contains(std::string("9629090436824982610")));
  ASSERT_TRUE(pathCache.contains(std::string("11453755272124169963")));
  ASSERT_TRUE(pathCache.contains(std::string("16582467240682114478")));

  ASSERT_TRUE(pathCache.contains(Path("a/a/c")));
  ASSERT_TRUE(pathCache.contains(Path("a/b/c")));
  ASSERT_TRUE(pathCache.contains(Path("a/c")));
}
