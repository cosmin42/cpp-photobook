#pragma once

#include <pb/util/Traits.h>

class PlatformHelper {
public:
  static Path CurrentAppLocation();

  static Path CurrentAppInstallationFolder();
};
