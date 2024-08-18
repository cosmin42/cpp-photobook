#pragma once

#include <pb/Platform.h>

DECLARE_STRONG_STRING(LutName)

namespace PB {
class LutService {
public:
  ~LutService() = default;

  void configurePlatformInfo(std::shared_ptr<PlatformInfo> platformInfo);

  std::unordered_set<PBDev::LutName, boost::hash<PBDev::LutName>>
  listLuts() const;

private:
  std::shared_ptr<PlatformInfo> mPlatformInfo;

  Path mLutAssetsPath() const;
};
} // namespace PB
