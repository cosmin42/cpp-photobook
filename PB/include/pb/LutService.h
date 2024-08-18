#pragma once

#include <pb/Platform.h>

namespace PB {
class LutService {
public:
  ~LutService() = default;

  void configurePlatformInfo(std::shared_ptr<PlatformInfo> platformInfo);

private:
  std::shared_ptr<PlatformInfo> mPlatformInfo;

  Path mLutAssetsPath() const;
};
} // namespace PB
