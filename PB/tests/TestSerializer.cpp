#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <boost/bimap/bimap.hpp>
#include <boost/uuid/random_generator.hpp>
#include <boost/uuid/uuid_io.hpp>

#include <pb/components/Serializer.h>
#include <pb/entities/PaperSettings.h>
#include <pb/entities/RegularImageV2.h>

TEST(TestSerializer, TestSimple)
{
  Json serialized;
  to_json(serialized, 1);
  std::string str = serialized.dump();
  ASSERT_EQ(str, "1");
  int deserialized;
  from_json(serialized, deserialized);
  ASSERT_EQ(deserialized, 1);

  std::string str1 = "string";
  to_json(serialized, str1);
  str = serialized.dump();
  ASSERT_EQ(str, "\"string\"");
  std::string deserialized1;
  from_json(serialized, deserialized1);
  ASSERT_EQ(deserialized1, "string");

  bool b = true;
  to_json(serialized, b);
  str = serialized.dump();
  ASSERT_EQ(str, "true");
  bool deserialized2;
  from_json(serialized, deserialized2);
  ASSERT_EQ(deserialized2, true);

  double d = 1.0;
  to_json(serialized, d);
  str = serialized.dump();
  ASSERT_EQ(str, "1.0");
  double deserialized3;
  from_json(serialized, deserialized3);
  ASSERT_EQ(deserialized3, 1.0);

  boost::uuids::uuid uuid = boost::uuids::random_generator()();
  to_json(serialized, uuid);
  str = serialized.dump();
  ASSERT_EQ(str, "\"" + boost::uuids::to_string(uuid) + "\"");
  boost::uuids::uuid deserialized4;
  from_json(serialized, deserialized4);
  ASSERT_EQ(deserialized4, uuid);
}

TEST(TestSerializer, TestArray)
{
  std::vector<int> v0 = {1, 2, 3};
  Json             serialized;
  to_json(serialized, v0);
  std::string str = serialized.dump();
  ASSERT_EQ(str, "[1,2,3]");
  std::vector<int> deserialized;
  from_json(serialized, deserialized);
  ASSERT_EQ(deserialized, v0);

  std::vector<std::string> v1 = {"a", "b", "c"};
  to_json(serialized, v1);
  str = serialized.dump();
  ASSERT_EQ(str, "[\"a\",\"b\",\"c\"]");
  std::vector<std::string> deserialized1;
  from_json(serialized, deserialized1);
  ASSERT_EQ(deserialized1, v1);

  std::vector<std::vector<int>> v2 = {{1, 2, 3}, {4, 5, 6}};
  to_json(serialized, v2);
  str = serialized.dump();
  ASSERT_EQ(str, "[[1,2,3],[4,5,6]]");
  std::vector<std::vector<int>> deserialized2;
  from_json(serialized, deserialized2);
  ASSERT_EQ(deserialized2, v2);

  std::vector<std::vector<Path>> v3 = {{Path("a"), Path("b"), Path("c")},
                                       {Path("d"), Path("e"), Path("f")}};
  to_json(serialized, v3);
  str = serialized.dump();
  ASSERT_EQ(str, "[[\"a\",\"b\",\"c\"],[\"d\",\"e\",\"f\"]]");
  std::vector<std::vector<Path>> deserialized3;
  from_json(serialized, deserialized3);
  ASSERT_EQ(deserialized3, v3);
}

TEST(TestSerializer, TestMap)
{
  std::unordered_map<std::string, int> m0 = {{"a", 1}, {"b", 2}, {"c", 3}};
  Json                                 serialized;
  to_json(serialized, m0);
  std::string str = serialized.dump();
  ASSERT_EQ(str, "{\"a\":1,\"b\":2,\"c\":3}");
  std::unordered_map<std::string, int> deserialized;
  from_json(serialized, deserialized);
  ASSERT_EQ(deserialized, m0);

  boost::bimaps::bimap<std::string, int> m1;
  m1.insert({"a", 1});
  m1.insert({"b", 2});
  m1.insert({"c", 3});
  to_json(serialized, m1);
  str = serialized.dump();
  ASSERT_EQ(str, "{\"a\":1,\"b\":2,\"c\":3}");
  boost::bimaps::bimap<std::string, int> deserialized1;
  from_json(serialized, deserialized1);
  ASSERT_EQ(deserialized1, m1);
}

TEST(TestSerializer, TestGenericImage)
{
  PB::GenericImagePtr genericImage =
      std::make_shared<PB::RegularImageV2>("hash", Path("original"));
  Json json;
  to_json(json, genericImage);
  std::string str = json.dump();
  ASSERT_EQ(str, "{\"hash\":\"hash\",\"original\":\"original\",\"type\":2}");
  PB::GenericImagePtr deserialized;
  from_json(json, deserialized);
  ASSERT_EQ(deserialized->hash(), "hash");
  ASSERT_EQ(deserialized->type(), PB::ImageType::Regular);
}

TEST(TestSerializer, TestStagedAndUnstaged)
{
  std::vector<PB::GenericImagePtr> staged;
  staged.push_back(
      std::make_shared<PB::RegularImageV2>("hash0", Path("original")));
  staged.push_back(
      std::make_shared<PB::RegularImageV2>("hash1", Path("original")));

  Json json;
  to_json(json, staged);
  std::string str = json.dump();
  ASSERT_EQ(str, "[{\"hash\":\"hash0\",\"original\":\"original\",\"type\":2},{"
                 "\"hash\":\"hash1\",\"original\":\"original\",\"type\":2}]");
  std::vector<PB::GenericImagePtr> deserialized;
  from_json(json, deserialized);
  ASSERT_EQ(deserialized.size(), 2);
  ASSERT_EQ(deserialized[0]->hash(), "hash0");
  ASSERT_EQ(deserialized[0]->type(), PB::ImageType::Regular);
  ASSERT_EQ(deserialized[1]->hash(), "hash1");
  ASSERT_EQ(deserialized[1]->type(), PB::ImageType::Regular);

  PB::GenericImagePtrMatrix unstaged;
  unstaged.push_back(staged);
  unstaged.push_back(staged);
  Json json1;
  to_json(json1, unstaged);
  std::string str1 = json1.dump();
  ASSERT_EQ(str1, "[[{\"hash\":\"hash0\",\"original\":\"original\",\"type\":2},"
                  "{\"hash\":\"hash1\",\"original\":\"original\",\"type\":2}],["
                  "{\"hash\":\"hash0\",\"original\":\"original\",\"type\":2},{"
                  "\"hash\":\"hash1\",\"original\":\"original\",\"type\":2}]]");
  PB::GenericImagePtrMatrix deserialized1;
  from_json(json1, deserialized1);
  ASSERT_EQ(deserialized1.size(), 2);
  ASSERT_EQ(deserialized1[0].size(), 2);
  ASSERT_EQ(deserialized1[0][0]->hash(), "hash0");
  ASSERT_EQ(deserialized1[0][0]->type(), PB::ImageType::Regular);
  ASSERT_EQ(deserialized1[0][1]->hash(), "hash1");
  ASSERT_EQ(deserialized1[0][1]->type(), PB::ImageType::Regular);
  ASSERT_EQ(deserialized1[1].size(), 2);
  ASSERT_EQ(deserialized1[1][0]->hash(), "hash0");
  ASSERT_EQ(deserialized1[1][0]->type(), PB::ImageType::Regular);
  ASSERT_EQ(deserialized1[1][1]->hash(), "hash1");
  ASSERT_EQ(deserialized1[1][1]->type(), PB::ImageType::Regular);
}

TEST(TestSerializer, TestPaperSettings)
{
  PB::PaperSettings paperSettings;
  paperSettings.type = PB::PaperType::A4_Landscape;
  paperSettings.ppi = 300;
  paperSettings.width = 2480;
  paperSettings.height = 3508;

  Json json;
  to_json(json, paperSettings);
  std::string str = json.dump();
  ASSERT_EQ(str, "{\"height\":3508,\"ppi\":300,\"type\":1,\"width\":2480}");
  PB::PaperSettings deserialized;
  from_json(json, deserialized);
  ASSERT_EQ(deserialized.type, PB::PaperType::A4_Landscape);
  ASSERT_EQ(deserialized.ppi, 300);
  ASSERT_EQ(deserialized.width, 2480);
  ASSERT_EQ(deserialized.height, 3508);
}
