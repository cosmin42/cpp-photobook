#pragma once

#include <pb/infra/Traits.h>

class PlatformHelper {
public:
  static Path CurrentAppLocation();

  static Path CurrentAppInstallationFolder();
};
