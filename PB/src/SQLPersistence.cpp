#include <pb/persistence/SQLPersistence.h>

namespace PB {

void SQLitePersistence::configure(Path localStatePath)
{
  mPath = localStatePath;
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

void SQLitePersistence::read(
    std::function<void(std::variant<MetadataPack, PBDev::Error>)> onReturn)
{
  auto maybeError = createProjectsRegisterIfNotExisting();
  if (maybeError.has_value()) {
    onReturn(maybeError.value());
    return;
  }

  MetadataPack metadataPack;

  sqlite3_stmt *stmt;
  auto success = sqlite3_prepare_v2(mDatabaseHandle.get(), SELECT_PROJECTS, -1,
                                    &stmt, nullptr);

  if (success != SQLITE_OK) {
    onReturn(PBDev::Error()
             << ErrorCode::SQLiteError << std::to_string(success));
    return;
  }

  while (sqlite3_step(stmt) == SQLITE_ROW) {
    const char *uuid =
        reinterpret_cast<const char *>(sqlite3_column_text(stmt, 1));
    const char *path =
        reinterpret_cast<const char *>(sqlite3_column_text(stmt, 2));
    metadataPack.metadata[uuid] = path;
  }

  sqlite3_finalize(stmt);
  onReturn(metadataPack);
}

void SQLitePersistence::write(
    std::pair<std::string, std::string>              entry,
    std::function<void(std::optional<PBDev::Error>)> onReturn)
{
  auto maybeError = createProjectsRegisterIfNotExisting();
  if (maybeError.has_value()) {
    onReturn(maybeError.value());
    return;
  }

  auto &[key, value] = entry;
  auto maybePairOrError = queryProjectEntry(key);

  if (std::holds_alternative<PBDev::Error>(maybePairOrError)) {
    onReturn(std::get<PBDev::Error>(maybePairOrError));
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
      onReturn(PBDev::Error() << ErrorCode::SQLiteError);
      return;
    }
    onReturn(std::nullopt);
  }
}

void SQLitePersistence::deleteEntry(
    std::string key, std::function<void(std::optional<PBDev::Error>)> onReturn)
{
  sqlite3_stmt *stmt;
  auto success = sqlite3_prepare_v2(mDatabaseHandle.get(), SELECT_PROJECTS, -1,
                                    &stmt, nullptr);

  if (success != SQLITE_OK) {
    onReturn(PBDev::Error()
             << ErrorCode::SQLiteError << std::to_string(success));
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
        onReturn(PBDev::Error() << ErrorCode::SQLiteError);
        return;
      }
    }
  }

  sqlite3_finalize(stmt);
  onReturn(std::nullopt);
}

void SQLitePersistence::write(
    MetadataPack                                     metadataPack,
    std::function<void(std::optional<PBDev::Error>)> onReturn)
{
  auto maybeError = createProjectsRegisterIfNotExisting();
  if (maybeError.has_value()) {
    onReturn(maybeError.value());
    return;
  }

  for (auto &[key, value] : metadataPack.metadata) {
    write(std::pair<std::string, std::string>(key, value),
          [onReturn](std::optional<PBDev::Error> maybeError) {
            onReturn(maybeError);
          });
  }

  sqlite3_stmt *stmt;
  auto success = sqlite3_prepare_v2(mDatabaseHandle.get(), SELECT_PROJECTS, -1,
                                    &stmt, nullptr);

  if (success != SQLITE_OK) {
    onReturn(PBDev::Error()
             << ErrorCode::SQLiteError << std::to_string(success));
    return;
  }

  while (sqlite3_step(stmt) == SQLITE_ROW) {
    const char *uuid =
        reinterpret_cast<const char *>(sqlite3_column_text(stmt, 1));
    if (metadataPack.metadata.find(uuid) == metadataPack.metadata.end()) {
      char *errMsg = nullptr;

      std::string query = "DELETE FROM PROJECTS_REGISTER WHERE uuid = '" +
                          std::string(uuid) + "';";

      success = sqlite3_exec(mDatabaseHandle.get(), query.c_str(), nullptr,
                             nullptr, &errMsg);

      if (success != SQLITE_OK) {
        sqlite3_free(errMsg);
        onReturn(PBDev::Error() << ErrorCode::SQLiteError);
        return;
      }
    }
  }

  sqlite3_finalize(stmt);

  onReturn(std::nullopt);
}

std::optional<PBDev::Error> SQLitePersistence::createTable(const char *request)
{
  char *errMsg = nullptr;
  auto  success =
      sqlite3_exec(mDatabaseHandle.get(), request, nullptr, nullptr, &errMsg);

  if (success != SQLITE_OK) {
    sqlite3_free(errMsg);
    return PBDev::Error() << ErrorCode::SQLiteError << std::to_string(success);
  }
  return std::nullopt;
}

std::optional<PBDev::Error>
SQLitePersistence::createProjectsRegisterIfNotExisting()
{
  auto maybeError = createTable(CREATE_PROJECTS_REGISTER);
  if (maybeError) {
    return maybeError;
  }

  maybeError = createTable(CREATE_PATH_CACHE_TABLE);
  if (maybeError) {
    return maybeError;
  }

  maybeError = createTable(CREATE_PROJECT_PATH_TABLE);
  if (maybeError) {
    return maybeError;
  }

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