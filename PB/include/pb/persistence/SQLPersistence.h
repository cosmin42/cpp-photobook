#pragma once

#include <optional>
#include <variant>

#include <boost/bimap/bimap.hpp>

#include <sqlite3.h>

#include <pb/Config.h>
#include <pb/util/Util.h>

namespace PB {

struct MetadataPack {
  std::unordered_map<std::string, std::string> metadata;
  boost::bimaps::bimap<Path, std::string>      pathCache;
  std::unordered_set<std::pair<Path, std::string>, PathProjectAssociationHash>
      pathProject;
};

class SQLitePersistence final {
public:
  static constexpr const char *DATABASE_NAME = "database.db";

  void configure(Path localStatePath);

  std::optional<PBDev::Error> connect();

  void
  read(std::function<void(std::variant<MetadataPack, PBDev::Error>)> onReturn);

  void write(std::pair<std::string, std::string>              entry,
             std::function<void(std::optional<PBDev::Error>)> onReturn);
  void write(MetadataPack                                     metadataPack,
             std::function<void(std::optional<PBDev::Error>)> onReturn);

  void deleteEntry(std::string                                      key,
                   std::function<void(std::optional<PBDev::Error>)> onReturn);

private:
  std::optional<PBDev::Error> createProjectsRegisterIfNotExisting();

  std::variant<std::optional<std::pair<std::string, std::string>>, PBDev::Error>
  queryProjectEntry(std::string searchedUUID);

  std::optional<PBDev::Error> createTable(const char *request);

  static constexpr const char *CREATE_PROJECTS_REGISTER =
      "CREATE TABLE IF NOT EXISTS PROJECTS_REGISTER ("
      "    id INTEGER PRIMARY KEY AUTOINCREMENT,"
      "    uuid TEXT NOT NULL,"
      "    path TEXT"
      ");";

  static constexpr const char *CREATE_PATH_CACHE_TABLE =
      "CREATE TABLE IF NOT EXISTS PROJECTS_REGISTER ("
      "    id INTEGER PRIMARY KEY AUTOINCREMENT,"
      "    path TEXT NOT NULL,"
      "    cache_path TEXT NOT NULL"
      ");";

  static constexpr const char *CREATE_PROJECT_PATH_TABLE =
      "CREATE TABLE IF NOT EXISTS PROJECTS_REGISTER ("
      "    id INTEGER PRIMARY KEY AUTOINCREMENT,"
      "    path TEXT NOT NULL,"
      "    project_name TEXT NOT NULL"
      ");";

  static constexpr const char *SELECT_PROJECTS =
      "SELECT * FROM PROJECTS_REGISTER;";

  Path                     mPath;
  std::shared_ptr<sqlite3> mDatabaseHandle = nullptr;
};
} // namespace PB