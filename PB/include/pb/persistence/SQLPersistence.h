#pragma once

#include <optional>
#include <variant>

#include <boost/bimap/bimap.hpp>

#include <sqlite3.h>

#include <pb/Config.h>
#include <pb/util/Util.h>

namespace PB {

class SQLitePersistenceListener {
public:
  virtual void
  onSQLiteMetadataRead(std::unordered_map<std::string, std::string> map) = 0;
  virtual void onSQLiteMetadataWritten() = 0;
  virtual void onSQLiteEntryDeleted() = 0;
  virtual void onSQLiteMetadataError(PBDev::Error) = 0;
};

class SQLitePersistence final {
public:
  static constexpr const char *DATABASE_NAME = "database.db";

  void configure(Path localStatePath);

  void configureSQLiteListener(SQLitePersistenceListener *);

  std::optional<PBDev::Error> connect();

  void read();

  void readPathCache(
      std::string uuid,
      std::function<void(
          std::variant<boost::bimaps::bimap<Path, std::string>, PBDev::Error>)>
          onReturn);

  std::variant<std::string, PBDev::Error> getPathHash(Path path);
  std::variant<bool, PBDev::Error>        hasHash(std::string hash);

  std::variant<std::string, PBDev::Error> pathHash(Path path);

  void write(std::pair<std::string, std::string> entry);
  void write(std::unordered_map<std::string, std::string> map);

  void deleteEntry(std::string key);

private:
  std::optional<PBDev::Error> createProjectsRegisterIfNotExisting();
  void                        maybeCreateHashPathsTable();

  std::variant<std::optional<std::pair<std::string, std::string>>, PBDev::Error>
  queryProjectEntry(std::string searchedUUID);

  static constexpr const char *CREATE_PROJECTS_REGISTER =
      "CREATE TABLE IF NOT EXISTS PROJECTS_REGISTER ("
      "    id INTEGER PRIMARY KEY AUTOINCREMENT,"
      "    uuid TEXT NOT NULL,"
      "    path TEXT"
      "    thumbnails TEXT"
      ");";

  static constexpr const char *CREATE_PATH_CACHE =
      "CREATE TABLE IF NOT EXISTS CACHE_REGISTER ("
      "    id INTEGER PRIMARY KEY AUTOINCREMENT,"
      "    uuid TEXT NOT NULL,"
      "    path TEXT NOT NULL"
      "    cache_path TEXT NOT NULL"
      ");";

  static constexpr const char *SELECT_PROJECTS =
      "SELECT * FROM PROJECTS_REGISTER;";
  SQLitePersistenceListener *mListener = nullptr;
  Path                       mPath;
  std::shared_ptr<sqlite3>   mDatabaseHandle = nullptr;
};
} // namespace PB