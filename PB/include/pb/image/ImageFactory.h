#pragma once

#include <pb/Platform.h>
#include <pb/entities/RegularImageV2.h>
#include <pb/entities/TextImageV2.h>
#include <pb/services/DurableHashService.h>
#include <pb/services/ProjectManagementService.h>

namespace PB {
class ImageFactory final {
public:
  ~ImageFactory() = default;

  void configurePlatformInfo(std::shared_ptr<PlatformInfo> platformInfo);
  void configureProject(IdentifiableProject project);
  void configureDurableHashService(std::shared_ptr<DurableCache> durableHashService);

  std::shared_ptr<RegularImageV2> createRegularImage(Path path);
  std::shared_ptr<RegularImageV2> createRegularImage(std::string hash);
  std::shared_ptr<TextImageV2>    createTextImage(Path path, std::string hash);

  GenericImagePtr copyImage(GenericImagePtr image);

  GenericImagePtr weakCopyImage(GenericImagePtr image);

  GenericImagePtr createImage(Path path);

  // TODO: Fix this if taking into account the platform info...
  GenericImagePtr defaultRegularImage()
  {
    if (mDefaultRegularImage == nullptr) {
      mDefaultRegularImage = std::make_shared<RegularImageV2>(
          RegularImageV2::defaultHash(), Path());
    }
    return mDefaultRegularImage;
  }

private:
  std::shared_ptr<PlatformInfo>       mPlatformInfo = nullptr;
  IdentifiableProject                 mProject = nullptr;
  std::shared_ptr<DurableCache>       mDurableHashService = nullptr;
  GenericImagePtr                     mDefaultRegularImage = nullptr;
};
} // namespace PB