#pragma once

#include <format>

#include <sqlite3.h>

#include <boost/bimap/bimap.hpp>

#include <pb/Platform.h>
#include <pb/ThreadScheduler.h>

namespace PB {

class DatabaseService final {
public:
  ~DatabaseService();
  void configurePlatformInfo(std::shared_ptr<PlatformInfo> platform);
  void configureThreadScheduler(
      std::shared_ptr<PBDev::ThreadScheduler> threadScheduler);

  void connect();

  static std::unordered_map<boost::uuids::uuid, std::string>
  deserializeProjectMetadata(std::vector<std::vector<std::string>> raw);

  static boost::bimaps::bimap<Path, std::string>
  deserializeCacheEntry(std::vector<std::vector<std::string>> raw);

private:
  sqlite3                                *mDatabase = nullptr;
  std::shared_ptr<PlatformInfo>           mPlatform = nullptr;
  std::shared_ptr<PBDev::ThreadScheduler> mThreadScheduler = nullptr;

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
      query += keys[i];
      if (i < N - 1) {
        query += ", ";
      }
    }
    query += ") VALUES (";
    for (int i = 0; i < N; i++) {
      query += values[i];
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
      query += keys[i] + " = " + values[i];
      if (i < N - 1) {
        query += ", ";
      }
    }
    query += ";";
    return query;
  }
};
} // namespace PB
