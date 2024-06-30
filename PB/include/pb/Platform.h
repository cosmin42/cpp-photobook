#pragma once

#include <pb/util/Util.h>

namespace PB {
struct PlatformInfo {
  Path                          installationPath;
  Path                          localStatePath;
  std::pair<unsigned, unsigned> screenSize;
};
} // namespace PB