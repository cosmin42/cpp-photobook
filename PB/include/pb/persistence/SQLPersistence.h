#pragma once

#include <optional>
#include <variant>

#include <boost/bimap/bimap.hpp>

#include <sqlite3.h>

#include <pb/Config.h>
#include <pb/util/Util.h>

namespace PB {

class SQLitePersistence final {
public:
  static constexpr const char *DATABASE_NAME = "database.db";

  void configure(Path localStatePath);

  std::optional<PBDev::Error> connect();

  void read(std::function<
            void(std::variant<std::unordered_map<std::string, std::string>,
                              PBDev::Error>)>
                onReturn);

  void readPathCache(
      std::string uuid,
      std::function<void(
          std::variant<boost::bimaps::bimap<Path, std::string>, PBDev::Error>)>
          onReturn);

  std::variant<std::string, PBDev::Error> getPathHash(Path path);
  std::variant<bool, PBDev::Error>        hasHash(std::string hash);

  std::variant<std::string, PBDev::Error> pathHash(Path path);

  void write(std::pair<std::string, std::string>              entry,
             std::function<void(std::optional<PBDev::Error>)> onReturn);
  void write(std::unordered_map<std::string, std::string>     map,
             std::function<void(std::optional<PBDev::Error>)> onReturn);

  void deleteEntry(std::string                                      key,
                   std::function<void(std::optional<PBDev::Error>)> onReturn);

private:
  std::optional<PBDev::Error> createProjectsRegisterIfNotExisting();
  std::optional<PBDev::Error> createPathCacheRegisterIfNotExisting();

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

  Path                     mPath;
  std::shared_ptr<sqlite3> mDatabaseHandle = nullptr;
};
} // namespace PB