#include <pb/persistence/SQLPersistence.h>

namespace PB {

void SQLitePersistence::configure(Path localStatePath)
{
  mPath = localStatePath;
}

void SQLitePersistence::configureSQLiteListener(
    SQLitePersistenceListener *listener)
{
  mListener = listener;
}

std::optional<PBDev::Error> SQLitePersistence::connect()
{
  auto databaseFilePath = mPath / DATABASE_NAME;

  sqlite3 *rawHandler = nullptr;
  auto success = sqlite3_open(databaseFilePath.string().c_str(), &rawHandler);
  if (success == SQLITE_OK) {
    mDatabaseHandle.reset(rawHandler,
                          [](sqlite3 *pointer) { sqlite3_close(pointer); });
    return std::nullopt;
  }
  return PBDev::Error() << ErrorCode::SQLiteError << std::to_string(success);
}

void SQLitePersistence::read()
{
  auto maybeError = createProjectsRegisterIfNotExisting();
  if (maybeError.has_value()) {
    mListener->onSQLiteMetadataError(maybeError.value());
    return;
  }

  std::unordered_map<std::string, std::string> readMap;

  sqlite3_stmt *stmt;
  auto success = sqlite3_prepare_v2(mDatabaseHandle.get(), SELECT_PROJECTS, -1,
                                    &stmt, nullptr);

  if (success != SQLITE_OK) {
    mListener->onSQLiteMetadataError(PBDev::Error() << ErrorCode::SQLiteError
                                                    << std::to_string(success));
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
  mListener->onSQLiteMetadataRead(readMap);
}

void SQLitePersistence::readPathCache(
    std::string uuid,
    std::function<void(
        std::variant<boost::bimaps::bimap<Path, std::string>, PBDev::Error>)>
        onReturn)
{
  maybeCreateHashPathsTable();

  boost::bimaps::bimap<Path, std::string> readMap;

  sqlite3_stmt *stmt;
  std::string   selectionQuery =
      "SELECT * FROM CACHE_REGISTER WHERE uuid = ' " + uuid + "';";
  auto success = sqlite3_prepare_v2(mDatabaseHandle.get(),
                                    selectionQuery.c_str(), -1, &stmt, nullptr);

  if (success != SQLITE_OK) {
    onReturn(PBDev::Error()
             << ErrorCode::SQLiteError << std::to_string(success));
    return;
  }

  while (sqlite3_step(stmt) == SQLITE_ROW) {
    const char *path =
        reinterpret_cast<const char *>(sqlite3_column_text(stmt, 1));
    const char *pathHash =
        reinterpret_cast<const char *>(sqlite3_column_text(stmt, 2));
    readMap.insert({Path(path), pathHash});
  }

  sqlite3_finalize(stmt);
  onReturn(readMap);
}

std::variant<bool, PBDev::Error> SQLitePersistence::hasHash(std::string hash)
{
  maybeCreateHashPathsTable();

  sqlite3_stmt *stmt;
  std::string   selectionQuery =
      "SELECT * FROM CACHE_REGISTER WHERE cache_path = ' " + hash + "';";
  auto success = sqlite3_prepare_v2(mDatabaseHandle.get(),
                                    selectionQuery.c_str(), -1, &stmt, nullptr);

  if (success != SQLITE_OK) {
    return PBDev::Error() << ErrorCode::SQLiteError << std::to_string(success);
  }

  while (sqlite3_step(stmt) == SQLITE_ROW) {
    sqlite3_finalize(stmt);
    return true;
  }
  sqlite3_finalize(stmt);
  return false;
}

std::variant<std::string, PBDev::Error>
SQLitePersistence::getPathHash(Path path)
{
  maybeCreateHashPathsTable();

  sqlite3_stmt *stmt;
  std::string   selectionQuery =
      "SELECT * FROM CACHE_REGISTER WHERE path = ' " + path.string() + "';";
  auto success = sqlite3_prepare_v2(mDatabaseHandle.get(),
                                    selectionQuery.c_str(), -1, &stmt, nullptr);

  if (success != SQLITE_OK) {
    return PBDev::Error() << ErrorCode::SQLiteError << std::to_string(success);
  }

  while (sqlite3_step(stmt) == SQLITE_ROW) {
    const char *pathHash =
        reinterpret_cast<const char *>(sqlite3_column_text(stmt, 3));
    sqlite3_finalize(stmt);
    return pathHash;
  }
  sqlite3_finalize(stmt);
  return PBDev::Error();
}

std::variant<std::string, PBDev::Error> SQLitePersistence::pathHash(Path path)
{
  maybeCreateHashPathsTable();

  int salt = 0;

  static constexpr int MAX_ITERATIONS = 200;

  std::string hash_s =
      std::to_string(std::hash<std::string>{}(path.filename().string()));

  while (salt < MAX_ITERATIONS) {
    auto complete_hash = hash_s + std::to_string(salt);
    auto hasHashOrError = hasHash(complete_hash);
    assert(std::holds_alternative<bool>(hasHashOrError));
    auto hasHash = std::get<bool>(hasHashOrError);
    if (!hasHash) {
      return complete_hash;
    }
    salt++;
  }
#if defined(_MSC_VER) && !defined(__clang__) // MSVC
  __assume(false);
#else // GCC, Clang
  __builtin_unreachable();
#endif
}

void SQLitePersistence::write(std::pair<std::string, std::string> entry)
{
  auto maybeError = createProjectsRegisterIfNotExisting();
  if (maybeError.has_value()) {
    mListener->onSQLiteMetadataError(maybeError.value());
    return;
  }

  auto &[key, value] = entry;
  auto maybePairOrError = queryProjectEntry(key);

  if (std::holds_alternative<PBDev::Error>(maybePairOrError)) {
    mListener->onSQLiteMetadataError(std::get<PBDev::Error>(maybePairOrError));
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
      mListener->onSQLiteMetadataError(PBDev::Error()
                                       << ErrorCode::SQLiteError);
      return;
    }
    mListener->onSQLiteMetadataWritten();
  }
}

void SQLitePersistence::deleteEntry(std::string key)
{
  sqlite3_stmt *stmt;
  auto success = sqlite3_prepare_v2(mDatabaseHandle.get(), SELECT_PROJECTS, -1,
                                    &stmt, nullptr);

  if (success != SQLITE_OK) {
    mListener->onSQLiteMetadataError(PBDev::Error() << ErrorCode::SQLiteError
                                                    << std::to_string(success));
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
        mListener->onSQLiteMetadataError(PBDev::Error()
                                         << ErrorCode::SQLiteError);
        return;
      }
    }
  }

  sqlite3_finalize(stmt);
  mListener->onSQLiteEntryDeleted();
}

void SQLitePersistence::write(std::unordered_map<std::string, std::string> map)
{
  auto maybeError = createProjectsRegisterIfNotExisting();
  if (maybeError.has_value()) {
    mListener->onSQLiteMetadataError(maybeError.value());
    return;
  }

  for (auto &[key, value] : map) {
    write(std::pair<std::string, std::string>(key, value));
  }

  sqlite3_stmt *stmt;
  auto success = sqlite3_prepare_v2(mDatabaseHandle.get(), SELECT_PROJECTS, -1,
                                    &stmt, nullptr);

  if (success != SQLITE_OK) {
    mListener->onSQLiteMetadataError(PBDev::Error() << ErrorCode::SQLiteError
                                                    << std::to_string(success));
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
        mListener->onSQLiteMetadataError(PBDev::Error()
                                         << ErrorCode::SQLiteError);
        return;
      }
    }
  }

  sqlite3_finalize(stmt);
  mListener->onSQLiteMetadataWritten();
}

std::optional<PBDev::Error>
SQLitePersistence::createProjectsRegisterIfNotExisting()
{
  char *errMsg = nullptr;
  auto  success = sqlite3_exec(mDatabaseHandle.get(), CREATE_PROJECTS_REGISTER,
                               nullptr, nullptr, &errMsg);

  if (success != SQLITE_OK) {
    sqlite3_free(errMsg);
    return PBDev::Error() << ErrorCode::SQLiteError << std::to_string(success);
  }

  return std::nullopt;
}

void SQLitePersistence::maybeCreateHashPathsTable()
{
  char *errMsg = nullptr;
  auto success = sqlite3_exec(mDatabaseHandle.get(), CREATE_PATH_CACHE, nullptr,
                              nullptr, &errMsg);
  sqlite3_free(errMsg);
  PBDev::basicAssert(success == SQLITE_OK);
}

std::string SQLitePersistence::computeHash(std::string key)
{
  return std::to_string(std::hash<std::string>{}(key));
}

bool SQLitePersistence::tryHash(std::string hash) { return true; }

std::string SQLitePersistence::saltHash(std::string hash)
{
  static constexpr int MAX_HASH_ATTEMPTS = 200;
  for (int i = 0; i < MAX_HASH_ATTEMPTS; ++i) {
    std::string hashAttempt = hash + std::to_string(i);
    if (tryHash(hashAttempt)) {
      return hashAttempt;
    }
  }
  PBDev::basicAssert(false);
  return "";
}

std::variant<std::optional<std::pair<std::string, std::string>>, PBDev::Error>
SQLitePersistence::queryProjectEntry(std::string searchedUUID)
{
  std::string query =
      "SELECT * FROM PROJECTS_REGISTER WHERE uuid='" + searchedUUID + "'";

  sqlite3_stmt *stmt;
  auto success = sqlite3_prepare_v2(mDatabaseHandle.get(), query.c_str(), -1,
                                    &stmt, nullptr);

  if (success != SQLITE_OK) {
    return PBDev::Error() << ErrorCode::SQLiteError << std::to_string(success);
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
} // namespace PB