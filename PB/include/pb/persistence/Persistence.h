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

        auto success = sqlite3_exec(mDatabaseHandle.get(), query.c_str(),
                                    nullptr, nullptr, &errMsg);

        if (success != SQLITE_OK) {
          sqlite3_free(errMsg);
          onReturn(Error() << ErrorCode::SQLiteError);
          return;
        }
      }
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
    std::string query = "SELECT " + searchedUUID + " FROM PROJECTS_REGISTER";

    sqlite3_stmt *stmt;
    auto success = sqlite3_prepare_v2(mDatabaseHandle.get(), SELECT_PROJECTS,
                                      -1, &stmt, nullptr);

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

template <typename PersistenceType = void> class Persistence final {
public:
  Persistence() = default;
  Persistence(Persistence const &) = delete;
  Persistence(Persistence &&) noexcept = delete;
  Persistence &operator=(Persistence const &) = delete;
  ~Persistence() = default;

  static std::string localFolder() { return PersistenceType::localFolder(); }

  void write(std::function<void(std::optional<Error>)> f)
  {
    mPersistence.template write<std::unordered_map>(
        mCache, [f{f}](std::optional<Error> out) { f(out); });
  }

  void write(std::string directory, std::string fileName,
             std::function<void(std::optional<Error>)> f)
  {
    mPersistence.template write<std::unordered_map>(
        directory, fileName, mCache,
        [f{f}](std::optional<Error> out) { f(out); });
  }

  void write(Path directory, Path fileName,
             std::function<void(std::optional<Error>)> f)
  {
    mPersistence.template write<std::unordered_map>(
        directory, fileName, mCache,
        [f{f}](std::optional<Error> out) { f(out); });
  }

  void load(std::function<void(std::optional<Error>)> f)
  {
    mPersistence.load([f{f}, this](std::optional<Error> out) {
      if (!out) {
        mCache.clear();
        auto &cacheRef = mPersistence.data();
        mCache.insert(cacheRef.begin(), cacheRef.end());
        cacheRef.clear();
      }
      f(out);
    });
  }

  void
  load(Path path,
       std::function<void(
           std::variant<std::unordered_map<std::string, std::string>, Error>)>
           f)
  {
    std::ifstream     t(path);
    std::stringstream buffer;
    buffer << t.rdbuf();

    auto parsedMapOrError = mPersistence.parseData(buffer.str());
    f(parsedMapOrError);
  }

  std::unordered_map<std::string, std::string> &cache() { return mCache; }

private:
  std::unordered_map<std::string, std::string> mCache;
  PersistenceType                              mPersistence;
};

template <> class Persistence<void> final {
public:
  Persistence() = default;
  explicit Persistence(Path const &path) : mLocalFile(path) {}
  Persistence(Persistence const &) = delete;
  Persistence(Persistence &&) noexcept = delete;
  Persistence &operator=(Persistence const &) = delete;
  ~Persistence() = default;

  Persistence &setLocalFolder(Path const &path)
  {
    mLocalFile = path;
    return *this;
  }

  void write(std::function<void(std::optional<Error>)> f)
  {
    write(mLocalFile, f);
  }

  void write(std::string path, std::function<void(std::optional<Error>)> f)
  {
    write(Path(path), f);
  }

  void write(Path path, std::function<void(std::optional<Error>)> f)
  {
    std::ofstream ofs(path.string());

    if (!ofs.is_open()) {
      f(Error() << ErrorCode::FileDoesNotExist);
      return;
    }

    for (auto &[key, value] : mCache) {
      ofs << key << "\n" << value << "\n";
    }
    ofs.close();

    f(std::nullopt);
  }

  void load(std::function<void(std::optional<Error>)> f)
  {
    load(mLocalFile, f);
  }

  void load(Path path, std::function<void(std::optional<Error>)> f)
  {
    std::ifstream file(path);
    if (!file.is_open()) {
      f(Error() << ErrorCode::FileDoesNotExist);
      return;
    }
    std::stringstream buffer;
    buffer << file.rdbuf();
    auto parsedDataOrError = parseData(buffer.str());

    if (std::holds_alternative<Error>(parsedDataOrError)) {
      f(std::get<Error>(parsedDataOrError));
    }
    else {
      auto &newData = std::get<std::unordered_map<std::string, std::string>>(
          parsedDataOrError);
      mCache.insert(newData.begin(), newData.end());
    }
    f(std::nullopt);
  }

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

  std::unordered_map<std::string, std::string> &cache() { return mCache; }

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

  std::unordered_map<std::string, std::string> mCache;
  Path                                         mLocalFile;
};

} // namespace PB
