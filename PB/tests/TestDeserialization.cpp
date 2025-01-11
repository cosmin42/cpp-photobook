#include <gtest/gtest.h>

#include <queue>

#include <pb/components/SerializationStrategy.h>

#include "MockListeners.h"

using namespace PB;

TEST(TestDeserialization, TestGenericImageDeserialization)
{
  std::string jsonData = R"(
	{
      "imageMonitor": [
        [
          {
            "hash": "a58786a6-43dd-4bf2-9478-671e9b7966e4",
            "original": "C:\\Users\\ionut\\cpp-photobook\\tests-data\\Collection\\Maldives\\DSC_0051.JPG",
            "type": "Regular"
          },
          {
            "hash": "f19b3b46-dbe4-4fd7-b8f4-9f76b270c0d8",
            "original": "C:\\Users\\ionut\\cpp-photobook\\tests-data\\Collection\\Maldives\\DSC_0054.JPG",
            "type": "Regular"
          }
        ]
      ],
      "name": "Gleaming",
      "paperSettings": {
        "height": 2480,
        "ppi": 300,
        "type": 1,
        "width": 3508
      },
      "roots": [
        "C:\\Users\\ionut\\cpp-photobook\\tests-data\\Collection\\Maldives"
      ],
      "stagedImages": null
    }
  )";

  Json json = Json::parse(jsonData);

  auto deserializedOrError = PB::Text::deserialize<Project>(json);

  ASSERT_FALSE(std::holds_alternative<PBDev::Error>(deserializedOrError));

  auto project = std::get<Project>(deserializedOrError);

  ASSERT_EQ(project.name, "Gleaming");
  ASSERT_EQ(project.paperSettings.height, 2480);
  ASSERT_EQ(project.paperSettings.ppi, 300);
  ASSERT_EQ(project.paperSettings.width, 3508);
}