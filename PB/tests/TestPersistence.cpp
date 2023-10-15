#include <gtest/gtest.h>

#include <pb/Config.h>
#include <pb/persistence/Persistence.h>

using namespace PB;

template <typename T> void testReadWrite(std::string path)
{
  T persistence(path);

  persistence.connect();

  std::unordered_map<std::string, std::string> map{{"testKey", "testValue"}};

  persistence.write(map, [map, path](std::optional<PB::Error> maybeError) {
    ASSERT_TRUE(!maybeError.has_value());
    T persistence(path);
    persistence.connect();
    persistence.read(
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

TEST(TestPersistence, ReadWrite)
{
  std::filesystem::remove(SQLitePersistence::DATABASE_NAME);
  std::filesystem::remove(Context::PERSISTENCE_FILENAME);
  testReadWrite<PB::SQLitePersistence>(".");
  testReadWrite<PB::FilePersistence>(Context::PERSISTENCE_FILENAME);
}

template <typename T> void testUpdateMap(std::string path)
{
  T persistence(path);

  persistence.connect();

  std::unordered_map<std::string, std::string> map{{"testKey", "testValue"}};

  persistence.write(map, [map, path](std::optional<PB::Error> maybeError) {
    ASSERT_TRUE(!maybeError.has_value());
    T persistence(path);
    persistence.connect();
    persistence.read(
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

  persistence.write(map, [map, path](std::optional<PB::Error> maybeError) {
    ASSERT_TRUE(!maybeError.has_value());
    T persistence(path);
    persistence.connect();
    persistence.read(
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
  std::filesystem::remove(SQLitePersistence::DATABASE_NAME);
  std::filesystem::remove(Context::PERSISTENCE_FILENAME);
  testUpdateMap<PB::SQLitePersistence>(".");
  testUpdateMap<PB::FilePersistence>(Context::PERSISTENCE_FILENAME);
}

template <typename T> void testRemoveEntry(std::string path)
{
  T persistence(path);

  persistence.connect();

  std::unordered_map<std::string, std::string> map{{"testKey", "testValue"}};

  persistence.write(map, [map, path](std::optional<PB::Error> maybeError) {
    ASSERT_TRUE(!maybeError.has_value());
    T persistence(path);
    persistence.connect();
    persistence.read(
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

  persistence.write(map, [map, path](std::optional<PB::Error> maybeError) {
    ASSERT_TRUE(!maybeError.has_value());
    T persistence(path);
    persistence.connect();
    persistence.read(
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
  std::filesystem::remove(SQLitePersistence::DATABASE_NAME);
  std::filesystem::remove(Context::PERSISTENCE_FILENAME);
  testRemoveEntry<PB::SQLitePersistence>(".");
  testRemoveEntry<PB::FilePersistence>(Context::PERSISTENCE_FILENAME);
}

TEST(TestPersistence, InsertEntry)
{
  std::filesystem::remove(SQLitePersistence::DATABASE_NAME);
  std::filesystem::remove(Context::PERSISTENCE_FILENAME);

  PB::SQLitePersistence persistence(".");

  auto maybeError = persistence.connect();
  ASSERT_TRUE(!maybeError);

  std::unordered_map<std::string, std::string> map{{"testKey", "testValue"}};

  persistence.write(map, [map](std::optional<PB::Error> maybeError) {
    ASSERT_TRUE(!maybeError.has_value());
    PB::SQLitePersistence persistence(".");
    persistence.connect();
    persistence.read(
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

  std::pair<std::string, std::string> pair = {"testKey", "testValue2"};

  persistence.write(pair, [map](std::optional<PB::Error> maybeError) {
    ASSERT_TRUE(!maybeError.has_value());
    PB::SQLitePersistence persistence(".");
    persistence.connect();
    persistence.read(
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

TEST(TestPersistence, DeleteEntry)
{
  std::filesystem::remove(SQLitePersistence::DATABASE_NAME);
  std::filesystem::remove(Context::PERSISTENCE_FILENAME);

  PB::SQLitePersistence persistence(".");

  auto maybeError = persistence.connect();
  ASSERT_TRUE(!maybeError);

  std::unordered_map<std::string, std::string> map{{"testKey", "testValue"}};

  persistence.write(map, [map](std::optional<PB::Error> maybeError) {
    ASSERT_TRUE(!maybeError.has_value());
    PB::SQLitePersistence persistence(".");
    persistence.connect();
    persistence.read(
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
  map.clear();

  persistence.deleteEntry("testKey", [map](
                                         std::optional<PB::Error> maybeError) {
    ASSERT_TRUE(!maybeError.has_value());
    PB::SQLitePersistence persistence(".");
    persistence.connect();
    persistence.read(
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

TEST(TestPersistence, DeleteNonExistentEntry)
{
  std::filesystem::remove(SQLitePersistence::DATABASE_NAME);
  std::filesystem::remove(Context::PERSISTENCE_FILENAME);

  PB::SQLitePersistence persistence(".");

  auto maybeError = persistence.connect();
  ASSERT_TRUE(!maybeError);

  std::unordered_map<std::string, std::string> map{{"testKey", "testValue"}};

  persistence.write(map, [map](std::optional<PB::Error> maybeError) {
    ASSERT_TRUE(!maybeError.has_value());
    PB::SQLitePersistence persistence(".");
    persistence.connect();
    persistence.read(
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

  map["testKey"] = "testValue";

  persistence.deleteEntry("non ExistentKey", [map](
                                         std::optional<PB::Error> maybeError) {
    ASSERT_TRUE(!maybeError.has_value());
    PB::SQLitePersistence persistence(".");
    persistence.connect();
    persistence.read(
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