#pragma once

#include <format>

#include <sqlite3.h>

#include <boost/bimap/bimap.hpp>

#include <pb/Platform.h>
#include <pb/infra/TaskCruncher.h>
#include <pb/infra/ThreadScheduler.h>

namespace PB::Service {

class DatabaseService final {
public:
  ~DatabaseService();
  void configurePlatformInfo(std::shared_ptr<PlatformInfo> platform);

  void connect();
  void disconnect();

  static boost::bimaps::bimap<boost::uuids::uuid, std::string>
  deserializeProjectMetadata(std::vector<std::vector<std::string>> raw);

  static boost::bimaps::bimap<Path, std::string>
  deserializeCacheEntry(std::vector<std::vector<std::string>> raw);

  void maybeCreateTables();

  bool checkTableExists(std::string tableName);

  std::vector<std::vector<std::string>>
  selectData(std::string tableName, std::string predicate,
             unsigned expectedColumnsCount)
  {
    std::string query;
    if (predicate.empty()) {
      query = "SELECT * FROM " + tableName + ";";
    }
    else {
      query = "SELECT * FROM " + tableName + " WHERE " + predicate + ";";
    }
    sqlite3_stmt *stmt;
    auto          success =
        sqlite3_prepare_v2(mDatabase, query.c_str(), -1, &stmt, nullptr);

    if (success != SQLITE_OK) {
      return std::vector<std::vector<std::string>>{};
    }

    std::vector<std::vector<std::string>> result;

    while (sqlite3_step(stmt) == SQLITE_ROW) {
      std::vector<std::string> row;
      for (unsigned int i = 1; i < expectedColumnsCount + 1; ++i) {
        const char *data =
            reinterpret_cast<const char *>(sqlite3_column_text(stmt, i));
        row.push_back(data);
      }
      result.push_back(row);
    }

    sqlite3_finalize(stmt);
    return result;
  }

  void deleteData(std::string tableName, std::string predicate)
  {
    std::string query =
        "DELETE FROM " + tableName + " WHERE " + predicate + ";";
    char *errMsg = nullptr;
    auto  success =
        sqlite3_exec(mDatabase, query.c_str(), nullptr, nullptr, &errMsg);
    sqlite3_free(errMsg);
    PBDev::basicAssert(success == SQLITE_OK);
  }

  template <int N>
  void insert(std::string registerName, std::array<const char *, N> keys,
              std::array<const char *, N> values)
  {
    auto command = assembleDatabaseInsert<N>(registerName, keys, values);

    char *errMsg = nullptr;
    auto  success =
        sqlite3_exec(mDatabase, command.c_str(), nullptr, nullptr, &errMsg);
    sqlite3_free(errMsg);
    PBDev::basicAssert(success == SQLITE_OK);
  }

  template <int N>
  void update(std::string registerName, std::array<const char *, N> keys,
              std::array<const char *, N> values)
  {
    auto command = assembleDatabaseUpdate<N>(registerName, keys, values);

    char *errMsg = nullptr;
    auto  success =
        sqlite3_exec(mDatabase, command.c_str(), nullptr, nullptr, &errMsg);
    sqlite3_free(errMsg);
    PBDev::basicAssert(success == SQLITE_OK);
  }

private:
  sqlite3                      *mDatabase = nullptr;
  std::shared_ptr<PlatformInfo> mPlatform = nullptr;

  template <int N>
  void
  maybeCreateTable(std::pair<const char *, std::array<const char *, N>> data)
  {
    auto command = assembleDatabaseCreateTableQuery<N>(data.first, data.second);

    char *errMsg = nullptr;
    auto  success =
        sqlite3_exec(mDatabase, command.c_str(), nullptr, nullptr, &errMsg);
    sqlite3_free(errMsg);
    PBDev::basicAssert(success == SQLITE_OK);
  }

  template <int N>
  std::string
  assembleDatabaseCreateTableQuery(std::string                 registryName,
                                   std::array<const char *, N> headers)
  {
    std::string query = "CREATE TABLE IF NOT EXISTS " + registryName + " (";
    query += "id INTEGER PRIMARY KEY AUTOINCREMENT, ";
    for (int i = 0; i < N; i++) {
      query += headers[i];
      if (i < N - 1) {
        query += ", ";
      }
    }
    query += ");";
    return query;
  }

  template <int N> std::string assembleDatabaseSelect(std::string registryName)
  {

    std::string query = "SELECT ";
    for (int i = 0; i < N; i++) {
      query += std::format("{}", i);
      if (i < N - 1) {
        query += ", ";
      }
    }
    query += " FROM " + registryName + ";";
    return query;
  }

  template <int N>
  std::string assembleDatabaseInsert(std::string                 registryName,
                                     std::array<const char *, N> keys,
                                     std::array<const char *, N> values)
  {
    std::string query = "INSERT INTO " + registryName + " (";
    for (int i = 0; i < N; i++) {
      query += "'";
      query += keys[i];
      query += "'";
      if (i < N - 1) {
        query += ", ";
      }
    }
    query += ") VALUES (";
    for (int i = 0; i < N; i++) {
      query += "'";
      query += values[i];
      query += "'";
      if (i < N - 1) {
        query += ", ";
      }
    }
    query += ");";
    return query;
  }

  template <int N>
  std::string assembleDatabaseUpdate(std::string                 registryName,
                                     std::array<const char *, N> keys,
                                     std::array<const char *, N> values)
  {
    std::string query = "UPDATE " + registryName + " SET ";
    for (int i = 0; i < N; i++) {
      query += "'";
      query += keys[i];
      query += "'";
      query += "=";
      query += "'";
      query += values[i];
      query += "'";
      if (i < N - 1) {
        query += ", ";
      }
    }
    query += ";";
    return query;
  }
};
} // namespace PB
