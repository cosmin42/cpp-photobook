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

std::string SQLitePersistence::hash(Path path, std::string id)
{
  maybeCreateHashPathsTable();

  auto maybeHash = databaseHashByPath(path);

  if (maybeHash) {
    insertHash(id, path, maybeHash.value());
    return maybeHash.value();
  }
  auto coreHash = computeHash(path.string());
  auto hash = saltHash(coreHash);

  insertHash(id, path, hash);
  return hash;
}

boost::bimaps::bimap<Path, std::string>
SQLitePersistence::hashSet(std::string id)
{
  std::string query = "SELECT * FROM PROJECTS_REGISTER WHERE uuid='" + id + "'";

  sqlite3_stmt *stmt;
  auto success = sqlite3_prepare_v2(mDatabaseHandle.get(), query.c_str(), -1,
                                    &stmt, nullptr);

  if (success != SQLITE_OK) {
    return boost::bimaps::bimap<Path, std::string>{};
  }

  boost::bimaps::bimap<Path, std::string> set;

  while (sqlite3_step(stmt) == SQLITE_ROW) {
    const char *path =
        reinterpret_cast<const char *>(sqlite3_column_text(stmt, 2));
    const char *hash =
        reinterpret_cast<const char *>(sqlite3_column_text(stmt, 3));

    set.insert({Path(path), hash});
  }

  sqlite3_finalize(stmt);
  return set;
}

void SQLitePersistence::deleteHash(std::string id) {

}

void SQLitePersistence::insertHash(std::string id, Path path, std::string hash)
{
  std::string query;
  query = "INSERT INTO CACHE_REGISTER (uuid, path, cache_path) VALUES ('" + id +
          "', '" + path.string() + "'" + ", '" + hash + "');";

  char *errMsg = nullptr;

  auto success = sqlite3_exec(mDatabaseHandle.get(), query.c_str(), nullptr,
                              nullptr, &errMsg);

  if (success != SQLITE_OK) {
    sqlite3_free(errMsg);
    PBDev::basicAssert(false);
  }
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
  PBDev::basicAssert(!key.empty());
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

bool SQLitePersistence::tryHash(std::string hash)
{
  sqlite3_stmt *stmt;
  auto success = sqlite3_prepare_v2(mDatabaseHandle.get(), SELECT_PATH_CACHE,
                                    -1, &stmt, nullptr);
  PBDev::basicAssert(success == SQLITE_OK);

  if (sqlite3_step(stmt) == SQLITE_ROW) {
    sqlite3_finalize(stmt);
    return true;
  }

  sqlite3_finalize(stmt);
  return false;
}

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

std::optional<std::string> SQLitePersistence::databaseHashByPath(Path path)
{
  std::string query =
      "SELECT * FROM PROJECTS_REGISTER WHERE path='" + path.string() + "'";

  sqlite3_stmt *stmt;
  auto success = sqlite3_prepare_v2(mDatabaseHandle.get(), query.c_str(), -1,
                                    &stmt, nullptr);

  if (success != SQLITE_OK) {
    return std::nullopt;
  }

  while (sqlite3_step(stmt) == SQLITE_ROW) {
    const char *hash =
        reinterpret_cast<const char *>(sqlite3_column_text(stmt, 3));
    sqlite3_finalize(stmt);
    return hash;
  }

  sqlite3_finalize(stmt);
  return std::nullopt;
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