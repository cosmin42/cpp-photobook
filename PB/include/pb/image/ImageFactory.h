#pragma once

#include <pb/DurableHashService.h>
#include <pb/Platform.h>
#include <pb/ProjectManagementSystem.h>
#include <pb/image/RegularImage.h>
#include <pb/image/TextImage.h>
#include <pb/persistence/PersistenceService.h>

namespace PB {
class ImageFactory final {
public:
  ~ImageFactory() = default;

  void configurePlatformInfo(std::shared_ptr<PlatformInfo> platformInfo);
  void configureProjectManagementSystem(
      std::shared_ptr<ProjectManagementSystem> projectManagementSystem);
  void configureDurableHashService(
	  std::shared_ptr<DurableHashService> durableHashService);

  std::shared_ptr<RegularImage> createRegularImage(Path path);
  std::shared_ptr<TextImage>    createTextImage(Path path, Path hashPath);

  std::shared_ptr<VirtualImage> createImage(Path path);
  std::shared_ptr<VirtualImage> copyImage(std::shared_ptr<VirtualImage> image);
  std::shared_ptr<VirtualImage>
  mapImageToPaper(std::shared_ptr<VirtualImage> image, Path hashPath);

  // TODO: Fix this if taking into account the platform info...
  std::shared_ptr<VirtualImage> defaultRegularImage()
  {
    if (mDefaultRegularImage == nullptr) {
      mDefaultRegularImage =
          std::make_shared<RegularImage>(defaultImageFrontend());
    }
    return mDefaultRegularImage;
  }

  ImageResources defaultImageFrontend()
  {
    return {mPlatformInfo->installationPath /
                Path(OneConfig::LOADING_PHOTO_PLACEHOLDER),
            mPlatformInfo->installationPath /
                Path(OneConfig::LOADING_PHOTO_PLACEHOLDER),
            mPlatformInfo->installationPath /
                Path(OneConfig::PHOTO_TIMELINE_DEFAULT_IMAGE)};
  }

private:
  std::shared_ptr<PlatformInfo>            mPlatformInfo = nullptr;
  std::shared_ptr<ProjectManagementSystem> mProjectManagementSystem = nullptr;
  std::shared_ptr<DurableHashService>      mDurableHashService = nullptr;
  std::shared_ptr<VirtualImage>            mDefaultRegularImage = nullptr;
};
} // namespace PB