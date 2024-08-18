#include <pb/LutService.h>

namespace PB {

void LutService::configurePlatformInfo(
    std::shared_ptr<PlatformInfo> platformInfo)
{
  mPlatformInfo = platformInfo;
}

std::unordered_set<PBDev::LutName, boost::hash<PBDev::LutName>>
LutService::listLuts() const
{
  return std::unordered_set<PBDev::LutName, boost::hash<PBDev::LutName>>();
}

Path LutService::mLutAssetsPath() const
{
  return mPlatformInfo->installationPath / "luts";
}
} // namespace PB
