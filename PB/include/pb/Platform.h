#pragma once

#include <pb/Config.h>
#include <pb/util/Util.h>

namespace PB {
struct PlatformInfo {
  Path                          installationPath;
  Path                          localStatePath;
  std::pair<unsigned, unsigned> screenSize;

  Path projectPath(std::string name) const
  {
    return localStatePath / "projects" / (name + OneConfig::BOOK_EXTENSION);
  }
};
} // namespace PB