#include <pb/LutService.h>

namespace PB {

void LutService::configurePlatformInfo(
    std::shared_ptr<PlatformInfo> platformInfo)
{
  mPlatformInfo = platformInfo;
}

Path LutService::mLutAssetsPath() const
{
  return mPlatformInfo->installationPath / "luts";
}
} // namespace PB
