#pragma once

#include "ImageResources.h"
#include "VirtualImagePtr.g.h"

#include <pb/image/VirtualImage.h>

namespace winrt::PhotobookRuntimeComponent::implementation {
struct VirtualImagePtr : VirtualImagePtrT<VirtualImagePtr> {
  VirtualImagePtr() {}
  explicit VirtualImagePtr(std::shared_ptr<PB::VirtualImage> virtualImage)
      : mVirtualImage(virtualImage)
  {
  }
  ~VirtualImagePtr() = default;

  VirtualImageType Imagetype()
  {
    return (VirtualImageType)mVirtualImage->type();
  }

  Windows::Foundation::Collections::IVector<winrt::hstring> resources()
  {
    auto nativeResources = winrt::single_threaded_vector<winrt::hstring>();

    for (auto i = 0; i < mVirtualImage->resources().size(); ++i) {
      nativeResources.Append(
          winrt::to_hstring(mVirtualImage->resources().at(i).string()));
    }

    return nativeResources;
  }

  winrt::hstring keyPath()
  {
    return winrt::to_hstring(mVirtualImage->keyPath().string());
  }

  void setFullSizePath(winrt::hstring path)
  {
    mVirtualImage->setFullSizePath(winrt::to_string(path));
  }
  void setMediumSizePath(winrt::hstring path)
  {
    mVirtualImage->setMediumSizePath(winrt::to_string(path));
  }
  void setSmallSizePath(winrt::hstring path)
  {
    mVirtualImage->setSmallSizePath(winrt::to_string(path));
  }

  void setSizePath(winrt::hstring fullSizePath, winrt::hstring mediumSizePath,
                   winrt::hstring smallSizePath)
  {
    mVirtualImage->setSizePath(winrt::to_string(fullSizePath),
                               winrt::to_string(mediumSizePath),
                               winrt::to_string(smallSizePath));
  }

  PhotobookRuntimeComponent::ImageResources frontend()
  {
    return winrt::make<ImageResources>(mVirtualImage->frontend());
  }

  bool processed() { return mVirtualImage->processed(); }

  void finishProcessing() { mVirtualImage->finishProcessing(); }

  std::shared_ptr<PB::VirtualImage> unwrap() { return mVirtualImage; }

private:
  std::shared_ptr<PB::VirtualImage> mVirtualImage;
};
} // namespace winrt::PhotobookRuntimeComponent::implementation
