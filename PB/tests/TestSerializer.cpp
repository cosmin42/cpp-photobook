#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <boost/bimap/bimap.hpp>
#include <boost/uuid/random_generator.hpp>
#include <boost/uuid/uuid_io.hpp>

#include <pb/components/Serializer.h>
#include <pb/entities/RegularImageV2.h>

TEST(TestSerializer, TestSimple)
{
  auto errorOrJson = PB::flatAndTagSimple(0, "tag", 1);
  ASSERT_TRUE(std::holds_alternative<Json>(errorOrJson));
  Json json = std::get<Json>(errorOrJson);
  ASSERT_TRUE(json.contains("tag"));
  ASSERT_EQ(json["tag"], 1);

  errorOrJson = PB::flatAndTagSimple<std::string>(0, "tag", "string");
  ASSERT_TRUE(std::holds_alternative<Json>(errorOrJson));
  json = std::get<Json>(errorOrJson);
  ASSERT_TRUE(json.contains("tag"));
  ASSERT_EQ(json["tag"], "string");

  errorOrJson = PB::flatAndTagSimple<bool>(0, "tag", false);
  ASSERT_TRUE(std::holds_alternative<Json>(errorOrJson));
  json = std::get<Json>(errorOrJson);
  ASSERT_TRUE(json.contains("tag"));
  ASSERT_EQ(json["tag"], false);

  boost::uuids::uuid uuid = boost::uuids::random_generator()();

  errorOrJson = PB::flatAndTagSimple<boost::uuids::uuid>(0, "tag", uuid);
  ASSERT_TRUE(std::holds_alternative<Json>(errorOrJson));
  json = std::get<Json>(errorOrJson);
  ASSERT_TRUE(json.contains("tag"));
  ASSERT_EQ(json["tag"], boost::uuids::to_string(uuid));
}

TEST(TestSerializer, TestArray)
{
  std::vector<int> v0 = {1, 2, 3};
  auto             json = PB::flatMaybeContainer<std::vector<int>>(0, v0);
  ASSERT_TRUE(std::holds_alternative<Json>(json));
  Json json0 = std::get<Json>(json);
  ASSERT_TRUE(json0.is_array());
  ASSERT_EQ(json0.size(), 3);
  ASSERT_EQ(json0[0], 1);
  ASSERT_EQ(json0[1], 2);
  ASSERT_EQ(json0[2], 3);

  std::vector<std::string> v1 = {"a", "b", "c"};
  json = PB::flatMaybeContainer<std::vector<std::string>>(0, v1);
  ASSERT_TRUE(std::holds_alternative<Json>(json));
  Json json1 = std::get<Json>(json);
  ASSERT_TRUE(json1.is_array());
  ASSERT_EQ(json1.size(), 3);
  ASSERT_EQ(json1[0], "a");
  ASSERT_EQ(json1[1], "b");
  ASSERT_EQ(json1[2], "c");

  std::vector<std::vector<int>> v2 = {{1, 2, 3}, {4, 5, 6}};
  json = PB::flatMaybeContainer<std::vector<std::vector<int>>>(0, v2);
  ASSERT_TRUE(std::holds_alternative<Json>(json));
  Json json2 = std::get<Json>(json);
  ASSERT_TRUE(json2.is_array());
  ASSERT_EQ(json2.size(), 2);
  ASSERT_TRUE(json2[0].is_array());
  ASSERT_EQ(json2[0].size(), 3);
  ASSERT_EQ(json2[0][0], 1);
  ASSERT_EQ(json2[0][1], 2);
  ASSERT_EQ(json2[0][2], 3);
  ASSERT_TRUE(json2[1].is_array());
  ASSERT_EQ(json2[1].size(), 3);
  ASSERT_EQ(json2[1][0], 4);
  ASSERT_EQ(json2[1][1], 5);
  ASSERT_EQ(json2[1][2], 6);

  std::vector<std::vector<Path>> v3 = {{Path("a"), Path("b"), Path("c")},
                                       {Path("d"), Path("e"), Path("f")}};
  json = PB::flatMaybeContainer<std::vector<std::vector<Path>>>(0, v3);
  ASSERT_TRUE(std::holds_alternative<Json>(json));
  Json json3 = std::get<Json>(json);
  ASSERT_TRUE(json3.is_array());
  ASSERT_EQ(json3.size(), 2);
  ASSERT_TRUE(json3[0].is_array());
  ASSERT_EQ(json3[0].size(), 3);
  ASSERT_EQ(json3[0][0], "a");
  ASSERT_EQ(json3[0][1], "b");
  ASSERT_EQ(json3[0][2], "c");
  ASSERT_TRUE(json3[1].is_array());
  ASSERT_EQ(json3[1].size(), 3);
  ASSERT_EQ(json3[1][0], "d");
  ASSERT_EQ(json3[1][1], "e");
  ASSERT_EQ(json3[1][2], "f");
}

TEST(TestSerializer, TestMap)
{

  std::unordered_map<std::string, int> m0 = {{"a", 1}, {"b", 2}, {"c", 3}};

  auto json =
      PB::flatMaybeContainer<std::unordered_map<std::string, int>>(0, m0);
  ASSERT_TRUE(std::holds_alternative<Json>(json));
  Json json0 = std::get<Json>(json);
  ASSERT_TRUE(json0.is_object());
  ASSERT_EQ(json0.size(), 3);
  ASSERT_EQ(json0["a"], 1);
  ASSERT_EQ(json0["b"], 2);
  ASSERT_EQ(json0["c"], 3);

  boost::bimaps::bimap<std::string, int> m1;
  m1.insert({"a", 1});
  m1.insert({"b", 2});
  m1.insert({"c", 3});

  json = PB::flatMaybeContainer<boost::bimaps::bimap<std::string, int>>(0, m1);

  ASSERT_TRUE(std::holds_alternative<Json>(json));
  Json json1 = std::get<Json>(json);
  ASSERT_TRUE(json1.is_object());
  ASSERT_EQ(json1.size(), 3);
  ASSERT_EQ(json1["a"], 1);
  ASSERT_EQ(json1["b"], 2);
  ASSERT_EQ(json1["c"], 3);
}

TEST(TestSerializer, TestGenericImage)
{
  PB::GenericImagePtr genericImage = std::make_shared<PB::RegularImageV2>(
      Path("projectPath"), "hash", Path("original"));
  auto json = PB::flatAndTagSimple<PB::GenericImagePtr>(0, "image", genericImage);
  ASSERT_TRUE(std::holds_alternative<Json>(json));
  Json json0 = std::get<Json>(json);
  ASSERT_TRUE(json0.is_object());
  ASSERT_EQ(json0.size(), 1);
  ASSERT_TRUE(json0.contains("image"));
  ASSERT_TRUE(json0["image"].is_object());
  ASSERT_EQ(json0["image"].size(), 2);
  ASSERT_TRUE(json0["image"].contains("hash"));
  ASSERT_EQ(json0["image"]["hash"], "hash");
  ASSERT_TRUE(json0["image"].contains("type"));
  ASSERT_EQ(json0["image"]["type"], "Regular");
}

TEST(TestSerializer, TestStagedAndUnstaged)
{
      std::vector<PB::GenericImagePtr> staged;
  staged.push_back(std::make_shared<PB::RegularImageV2>(
	  Path("projectPath"), "hash0", Path("original")));
  staged.push_back(std::make_shared<PB::RegularImageV2>(
	  Path("projectPath"), "hash1", Path("original")));


  auto jsonOrError = PB::flatMaybeContainer<PB::GenericImagePtrLine>(0, staged);
  ASSERT_TRUE(std::holds_alternative<Json>(jsonOrError));
  Json json = std::get<Json>(jsonOrError);
  ASSERT_TRUE(json.is_array());
  ASSERT_EQ(json.size(), 2);
  ASSERT_TRUE(json[0].is_object());
  ASSERT_EQ(json[0].size(), 2);
  ASSERT_TRUE(json[0].contains("hash"));
  ASSERT_EQ(json[0]["hash"], "hash0");
  ASSERT_TRUE(json[0].contains("type"));
  ASSERT_EQ(json[0]["type"], "Regular");
  ASSERT_TRUE(json[1].is_object());
  ASSERT_EQ(json[1].size(), 2);
  ASSERT_TRUE(json[1].contains("hash"));
  ASSERT_EQ(json[1]["hash"], "hash1");
  ASSERT_TRUE(json[1].contains("type"));
  ASSERT_EQ(json[1]["type"], "Regular");

  PB::GenericImagePtrMatrix unstaged;
  unstaged.push_back(staged);
  unstaged.push_back(staged);
  jsonOrError = PB::flatMaybeContainer<PB::GenericImagePtrMatrix>(0, unstaged);
  ASSERT_TRUE(std::holds_alternative<Json>(jsonOrError));
  json = std::get<Json>(jsonOrError);
  ASSERT_TRUE(json.is_array());
  ASSERT_EQ(json.size(), 2);
  ASSERT_TRUE(json[0].is_array());
  ASSERT_EQ(json[0].size(), 2);
  ASSERT_TRUE(json[0][0].is_object());
  ASSERT_EQ(json[0][0].size(), 2);
  ASSERT_TRUE(json[0][0].contains("hash"));
  ASSERT_EQ(json[0][0]["hash"], "hash0");
  ASSERT_TRUE(json[0][0].contains("type"));
  ASSERT_EQ(json[0][0]["type"], "Regular");
  ASSERT_TRUE(json[0][1].is_object());
  ASSERT_EQ(json[0][1].size(), 2);
  ASSERT_TRUE(json[0][1].contains("hash"));
  ASSERT_EQ(json[0][1]["hash"], "hash1");
  ASSERT_TRUE(json[0][1].contains("type"));
  ASSERT_EQ(json[0][1]["type"], "Regular");
  ASSERT_TRUE(json[1].is_array());
  ASSERT_EQ(json[1].size(), 2);
  ASSERT_TRUE(json[1][0].is_object());
  ASSERT_EQ(json[1][0].size(), 2);
  ASSERT_TRUE(json[1][0].contains("hash"));
  ASSERT_EQ(json[1][0]["hash"], "hash0");
  ASSERT_TRUE(json[1][0].contains("type"));
  ASSERT_EQ(json[1][0]["type"], "Regular");
  ASSERT_TRUE(json[1][1].is_object());
  ASSERT_EQ(json[1][1].size(), 2);
  ASSERT_TRUE(json[1][1].contains("hash"));
  ASSERT_EQ(json[1][1]["hash"], "hash1");
  ASSERT_TRUE(json[1][1].contains("type"));
  ASSERT_EQ(json[1][1]["type"], "Regular");
}
