#pragma once

#include <fstream>
#include <functional>
#include <memory>
#include <optional>
#include <ranges>
#include <string>
#include <unordered_map>
#include <variant>

#include <pb/Error.h>
#include <pb/util/Traits.h>

#include <sqlite3.h>

namespace PB {

class SQLitePersistence final {
public:
  static constexpr const char *DATABASE_NAME = "database.db";

  explicit SQLitePersistence(Path path) : mPath(path) {}

  ~SQLitePersistence() = default;

  std::optional<Error> connect()
  {
    auto databaseFilePath = mPath / DATABASE_NAME;

    sqlite3 *rawHandler = nullptr;
    auto success = sqlite3_open(databaseFilePath.string().c_str(), &rawHandler);
    if (success == SQLITE_OK) {
      mDatabaseHandle.reset(rawHandler,
                            [](sqlite3 *pointer) { sqlite3_close(pointer); });
      return std::nullopt;
    }
    return Error() << ErrorCode::SQLiteError << std::to_string(success);
  }

  void
  read(std::function<
       void(std::variant<std::unordered_map<std::string, std::string>, Error>)>
           onReturn)
  {
    auto maybeError = createProjectsRegisterIfNotExisting();
    if (maybeError.has_value()) {
      onReturn(maybeError.value());
      return;
    }

    std::unordered_map<std::string, std::string> readMap;

    sqlite3_stmt *stmt;
    auto success = sqlite3_prepare_v2(mDatabaseHandle.get(), SELECT_PROJECTS,
                                      -1, &stmt, nullptr);

    if (success != SQLITE_OK) {
      onReturn(Error() << ErrorCode::SQLiteError << std::to_string(success));
      return;
    }

    while (sqlite3_step(stmt) == SQLITE_ROW) {
      const char *uuid =
          reinterpret_cast<const char *>(sqlite3_column_text(stmt, 1));
      const char *path =
          reinterpret_cast<const char *>(sqlite3_column_text(stmt, 2));
      readMap[uuid] = path;
    }

    sqlite3_finalize(stmt);
    onReturn(readMap);
  }

  void write(std::pair<std::string, std::string>       entry,
             std::function<void(std::optional<Error>)> onReturn)
  {
    auto maybeError = createProjectsRegisterIfNotExisting();
    if (maybeError.has_value()) {
      onReturn(maybeError.value());
      return;
    }

    auto &[key, value] = entry;
    auto maybePairOrError = queryProjectEntry(key);

    if (std::holds_alternative<Error>(maybePairOrError)) {
      onReturn(std::get<Error>(maybePairOrError));
    }
    else {
      auto &maybePair =
          std::get<std::optional<std::pair<std::string, std::string>>>(
              maybePairOrError);

      std::string query;
      if (maybePair.has_value()) {
        query = "UPDATE PROJECTS_REGISTER SET path = '" + value +
                "' WHERE uuid = '" + key + "';";
      }
      else {
        query = "INSERT INTO PROJECTS_REGISTER (uuid, path) VALUES ('" + key +
                "', '" + value + "');";
      }

      char *errMsg = nullptr;

      auto success = sqlite3_exec(mDatabaseHandle.get(), query.c_str(), nullptr,
                                  nullptr, &errMsg);

      if (success != SQLITE_OK) {
        sqlite3_free(errMsg);
        onReturn(Error() << ErrorCode::SQLiteError);
        return;
      }
      onReturn(std::nullopt);
    }
  }

  void deleteEntry(std::string                               key,
                   std::function<void(std::optional<Error>)> onReturn)
  {
    sqlite3_stmt *stmt;
    auto success = sqlite3_prepare_v2(mDatabaseHandle.get(), SELECT_PROJECTS,
                                      -1, &stmt, nullptr);

    if (success != SQLITE_OK) {
      onReturn(Error() << ErrorCode::SQLiteError << std::to_string(success));
      return;
    }

    while (sqlite3_step(stmt) == SQLITE_ROW) {
      const char *uuid =
          reinterpret_cast<const char *>(sqlite3_column_text(stmt, 1));
      if (uuid == key) {
        char *errMsg = nullptr;

        std::string query = "DELETE FROM PROJECTS_REGISTER WHERE uuid = '" +
                            std::string(uuid) + "';";

        success = sqlite3_exec(mDatabaseHandle.get(), query.c_str(), nullptr,
                               nullptr, &errMsg);

        if (success != SQLITE_OK) {
          sqlite3_free(errMsg);
          onReturn(Error() << ErrorCode::SQLiteError);
          return;
        }
      }
    }

    sqlite3_finalize(stmt);
    onReturn(std::nullopt);
  }

  void write(std::unordered_map<std::string, std::string> map,
             std::function<void(std::optional<Error>)>    onReturn)
  {
    auto maybeError = createProjectsRegisterIfNotExisting();
    if (maybeError.has_value()) {
      onReturn(maybeError.value());
      return;
    }

    for (auto &[key, value] : map) {
      write(std::pair<std::string, std::string>(key, value),
            [onReturn](std::optional<Error> maybeError) {
              onReturn(maybeError);
            });
    }

    sqlite3_stmt *stmt;
    auto success = sqlite3_prepare_v2(mDatabaseHandle.get(), SELECT_PROJECTS,
                                      -1, &stmt, nullptr);

    if (success != SQLITE_OK) {
      onReturn(Error() << ErrorCode::SQLiteError << std::to_string(success));
      return;
    }

    while (sqlite3_step(stmt) == SQLITE_ROW) {
      const char *uuid =
          reinterpret_cast<const char *>(sqlite3_column_text(stmt, 1));
      if (map.find(uuid) == map.end()) {
        char *errMsg = nullptr;

        std::string query = "DELETE FROM PROJECTS_REGISTER WHERE uuid = '" +
                            std::string(uuid) + "';";

        success = sqlite3_exec(mDatabaseHandle.get(), query.c_str(), nullptr,
                               nullptr, &errMsg);

        if (success != SQLITE_OK) {
          sqlite3_free(errMsg);
          onReturn(Error() << ErrorCode::SQLiteError);
          return;
        }
      }
    }

    sqlite3_finalize(stmt);

    onReturn(std::nullopt);
  }

private:
  std::optional<Error> createProjectsRegisterIfNotExisting()
  {
    char *errMsg = nullptr;
    auto success = sqlite3_exec(mDatabaseHandle.get(), CREATE_PROJECTS_REGISTER,
                                nullptr, nullptr, &errMsg);

    if (success != SQLITE_OK) {
      sqlite3_free(errMsg);
      return Error() << ErrorCode::SQLiteError << std::to_string(success);
    }

    return std::nullopt;
  }

  std::variant<std::optional<std::pair<std::string, std::string>>, Error>
  queryProjectEntry(std::string searchedUUID)
  {
    std::string query =
        "SELECT * FROM PROJECTS_REGISTER WHERE uuid='" + searchedUUID + "'";

    sqlite3_stmt *stmt;
    auto success = sqlite3_prepare_v2(mDatabaseHandle.get(), query.c_str(), -1,
                                      &stmt, nullptr);

    if (success != SQLITE_OK) {
      return Error() << ErrorCode::SQLiteError << std::to_string(success);
    }

    while (sqlite3_step(stmt) == SQLITE_ROW) {
      const char *uuid =
          reinterpret_cast<const char *>(sqlite3_column_text(stmt, 1));
      const char *path =
          reinterpret_cast<const char *>(sqlite3_column_text(stmt, 2));
      sqlite3_finalize(stmt);
      return std::pair<std::string, std::string>(uuid, path);
    }

    sqlite3_finalize(stmt);
    return std::nullopt;
  }

  static constexpr const char *CREATE_PROJECTS_REGISTER =
      "CREATE TABLE IF NOT EXISTS PROJECTS_REGISTER ("
      "    id INTEGER PRIMARY KEY AUTOINCREMENT,"
      "    uuid TEXT NOT NULL,"
      "    path TEXT"
      ");";

  static constexpr const char *SELECT_PROJECTS =
      "SELECT * FROM PROJECTS_REGISTER;";

  Path                     mPath;
  std::shared_ptr<sqlite3> mDatabaseHandle = nullptr;
};

class FilePersistence final {
public:
  static bool createDirectory(Path path)
  {
    if (std::filesystem::exists(path)) {
      return true;
    }

    if (!std::filesystem::create_directory(path)) {
      return false;
    }
    return true;
  }
  explicit FilePersistence(Path path) : mPath(path) {}
  std::optional<Error> connect() { return std::nullopt; }

  void
  read(std::function<
       void(std::variant<std::unordered_map<std::string, std::string>, Error>)>
           onReturn)
  {
    std::ifstream file(mPath);
    if (!file.is_open()) {
      onReturn(Error() << ErrorCode::FileDoesNotExist);
      return;
    }
    std::stringstream buffer;
    buffer << file.rdbuf();
    auto parsedDataOrError = parseData(buffer.str());

    if (std::holds_alternative<Error>(parsedDataOrError)) {
      onReturn(std::get<Error>(parsedDataOrError));
    }
    else {
      auto &newData = std::get<std::unordered_map<std::string, std::string>>(
          parsedDataOrError);
      onReturn(newData);
    }
  }

  void read(std::function<void(std::variant<Json, Error>)> onReturn)
  {
    std::ifstream file(mPath);
    if (!file.is_open()) {
      onReturn(Error() << ErrorCode::FileDoesNotExist);
      return;
    }
    Json jsonData;
    try {
      file >> jsonData;
      onReturn(jsonData);
    }
    catch (...) {
      onReturn(Error() << ErrorCode::JSONParseError);
    }
  }

  void write(std::unordered_map<std::string, std::string> map,
             std::function<void(std::optional<Error>)>    onReturn)
  {
    std::ofstream ofs(mPath.string());

    if (!ofs.is_open()) {
      onReturn(Error() << ErrorCode::FileDoesNotExist);
      return;
    }

    for (auto &[key, value] : map) {
      ofs << key << "\n" << value << "\n";
    }
    ofs.close();

    onReturn(std::nullopt);
  }

private:
  std::variant<std::unordered_map<std::string, std::string>, Error>
  parseData(std::string const &rawData)
  {
    std::unordered_map<std::string, std::string> parsed;
    auto                     tokensRanges = rawData | std::views::split('\n');
    std::vector<std::string> pair;
    for (const auto &tokenRange : tokensRanges) {
      auto newStr = std::string(tokenRange.begin(), tokenRange.end());
      if (newStr.empty()) {
        continue;
      }
      pair.push_back(newStr);
      if (pair.size() == 2) {
        parsed[pair.at(0)] = pair.at(1);
        pair.clear();
      }
    }
    if (pair.size() == 1) {
      return Error() << ErrorCode::CorruptPersistenceFile;
    }
    return parsed;
  }

  Path mPath;
};
} // namespace PB
