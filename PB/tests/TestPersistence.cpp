#include <gtest/gtest.h>

#include <pb/Config.h>
#include <pb/persistence/Persistence.h>

using namespace PB;

TEST(TestPersistence, ReadWrite)
{
  std::filesystem::remove(PB::Context::PERSISTENCE_FILENAME);

  PB::SQLitePersistence sqlitePersistence(".");

  sqlitePersistence.connect();

  std::unordered_map<std::string, std::string> map{{"testKey", "testValue"}};

  sqlitePersistence.write(map, [map](std::optional<PB::Error> maybeError) {
    ASSERT_TRUE(!maybeError.has_value());
    PB::SQLitePersistence sqlitePersistence(".");
    sqlitePersistence.connect();
    sqlitePersistence.read(
        [map](std::variant<std::unordered_map<std::string, std::string>, Error>
                  mapOrError) {
          ASSERT_TRUE(!std::holds_alternative<Error>(mapOrError));
          auto &secondMap =
              std::get<std::unordered_map<std::string, std::string>>(
                  mapOrError);
          ASSERT_TRUE(map.size() == secondMap.size());

          for (auto &[key, value] : map) {
            ASSERT_TRUE(secondMap.find(key) != secondMap.end());
            ASSERT_TRUE(secondMap.at(key) == map.at(key));
          }
        });
  });
}

TEST(TestPersistence, UpdateMap)
{
  std::filesystem::remove(PB::Context::PERSISTENCE_FILENAME);

  PB::SQLitePersistence sqlitePersistence(".");

  sqlitePersistence.connect();

  std::unordered_map<std::string, std::string> map{{"testKey", "testValue"}};

  sqlitePersistence.write(map, [map](std::optional<PB::Error> maybeError) {
    ASSERT_TRUE(!maybeError.has_value());
    PB::SQLitePersistence sqlitePersistence(".");
    sqlitePersistence.connect();
    sqlitePersistence.read(
        [map](std::variant<std::unordered_map<std::string, std::string>, Error>
                  mapOrError) {
          ASSERT_TRUE(!std::holds_alternative<Error>(mapOrError));
          auto &secondMap =
              std::get<std::unordered_map<std::string, std::string>>(
                  mapOrError);
          ASSERT_TRUE(map.size() == secondMap.size());

          for (auto &[key, value] : map) {
            ASSERT_TRUE(secondMap.find(key) != secondMap.end());
            ASSERT_TRUE(secondMap.at(key) == map.at(key));
          }
        });
  });

  map["testKey"] = "testValue2";

  sqlitePersistence.write(map, [map](std::optional<PB::Error> maybeError) {
    ASSERT_TRUE(!maybeError.has_value());
    PB::SQLitePersistence sqlitePersistence(".");
    sqlitePersistence.connect();
    sqlitePersistence.read(
        [map](std::variant<std::unordered_map<std::string, std::string>, Error>
                  mapOrError) {
          ASSERT_TRUE(!std::holds_alternative<Error>(mapOrError));
          auto &secondMap =
              std::get<std::unordered_map<std::string, std::string>>(
                  mapOrError);
          ASSERT_TRUE(map.size() == secondMap.size());

          for (auto &[key, value] : map) {
            ASSERT_TRUE(secondMap.find(key) != secondMap.end());
            ASSERT_TRUE(secondMap.at(key) == map.at(key));
          }
        });
  });
}

TEST(TestPersistence, RemoveEntry)
{
  std::filesystem::remove(PB::Context::PERSISTENCE_FILENAME);

  PB::SQLitePersistence sqlitePersistence(".");

  sqlitePersistence.connect();

  std::unordered_map<std::string, std::string> map{{"testKey", "testValue"}};

  sqlitePersistence.write(map, [map](std::optional<PB::Error> maybeError) {
    ASSERT_TRUE(!maybeError.has_value());
    PB::SQLitePersistence sqlitePersistence(".");
    sqlitePersistence.connect();
    sqlitePersistence.read(
        [map](std::variant<std::unordered_map<std::string, std::string>, Error>
                  mapOrError) {
          ASSERT_TRUE(!std::holds_alternative<Error>(mapOrError));
          auto &secondMap =
              std::get<std::unordered_map<std::string, std::string>>(
                  mapOrError);
          ASSERT_TRUE(map.size() == secondMap.size());

          for (auto &[key, value] : map) {
            ASSERT_TRUE(secondMap.find(key) != secondMap.end());
            ASSERT_TRUE(secondMap.at(key) == map.at(key));
          }
        });
  });

  map.clear();

  sqlitePersistence.write(map, [map](std::optional<PB::Error> maybeError) {
    ASSERT_TRUE(!maybeError.has_value());
    PB::SQLitePersistence sqlitePersistence(".");
    sqlitePersistence.connect();
    sqlitePersistence.read(
        [map](std::variant<std::unordered_map<std::string, std::string>, Error>
                  mapOrError) {
          ASSERT_TRUE(!std::holds_alternative<Error>(mapOrError));
          auto &secondMap =
              std::get<std::unordered_map<std::string, std::string>>(
                  mapOrError);
          ASSERT_TRUE(map.size() == secondMap.size());

          for (auto &[key, value] : map) {
            ASSERT_TRUE(secondMap.find(key) != secondMap.end());
            ASSERT_TRUE(secondMap.at(key) == map.at(key));
          }
        });
  });
}