#pragma once

#include <pb/Config.h>
#include <pb/util/Util.h>

namespace PB {
struct PlatformInfo {
  Path                          installationPath;
  Path                          localStatePath;
  std::pair<unsigned, unsigned> screenSize;

  Path projectFolderPath() const { return localStatePath / "projects"; }

  Path projectSupportFolder(boost::uuids::uuid projectId) const
  {
    return projectFolderPath() / boost::uuids::to_string(projectId);
  }

  Path projectPath(std::string name) const
  {
    return localStatePath / "projects" / (name + OneConfig::BOOK_EXTENSION);
  }

  Path databasePath() const
  {
    return localStatePath / OneConfig::DATABASE_NAME;
  }
};
} // namespace PB