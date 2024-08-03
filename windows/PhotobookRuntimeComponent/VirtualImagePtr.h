#pragma once

#include "ImageResources.h"
#include "VirtualImagePtr.g.h"

#include "Int32Pair.h"

#include <pb/image/Image.h>
#include <pb/image/ImageFactory.h>

#include <winrt/Microsoft.Graphics.Canvas.h>

namespace winrt::PhotobookRuntimeComponent::implementation {
struct VirtualImagePtr : VirtualImagePtrT<VirtualImagePtr> {

  explicit VirtualImagePtr(std::shared_ptr<PB::VirtualImage> virtualImage)
      : mVirtualImage(virtualImage)
  {
  }

  ~VirtualImagePtr() = default;

  PhotobookRuntimeComponent::Int32Pair Size()
  {
    return winrt::make<Int32Pair>(mVirtualImage->frontend().width,
                                  mVirtualImage->frontend().height);
  }

  VirtualImageType Imagetype()
  {
    return (VirtualImageType)mVirtualImage->type();
  }

  void GalleryProjection(winrt::array_view<uint8_t> buffer, int portviewWidth,
                         int portviewHeight)
  {
    auto mediumThumbnailPath = mVirtualImage->frontend().medium;

    auto tmpImage = PB::ImageReader().read(mediumThumbnailPath.string(), false,
                                           {portviewWidth, portviewHeight});

    std::copy_n((uint8_t *)tmpImage->data,
                (uint32_t)(tmpImage->total() * tmpImage->elemSize()),
                buffer.begin());
  }

  Windows::Foundation::Collections::IVector<winrt::hstring> Resources()
  {
    auto nativeResources = winrt::single_threaded_vector<winrt::hstring>();

    for (auto i = 0; i < mVirtualImage->resources().size(); ++i) {
      nativeResources.Append(
          winrt::to_hstring(mVirtualImage->resources().at(i).string()));
    }

    return nativeResources;
  }

  winrt::hstring KeyPath()
  {
    return winrt::to_hstring(mVirtualImage->keyPath().string());
  }

  void SetFullSizePath(winrt::hstring path)
  {
    mVirtualImage->setFullSizePath(winrt::to_string(path));
  }
  void SetMediumSizePath(winrt::hstring path)
  {
    mVirtualImage->setMediumSizePath(winrt::to_string(path));
  }
  void SetSmallSizePath(winrt::hstring path)
  {
    mVirtualImage->setSmallSizePath(winrt::to_string(path));
  }

  void SetSizePath(winrt::hstring fullSizePath, winrt::hstring mediumSizePath,
                   winrt::hstring smallSizePath)
  {
    mVirtualImage->setSizePath(winrt::to_string(fullSizePath),
                               winrt::to_string(mediumSizePath),
                               winrt::to_string(smallSizePath));
  }

  PhotobookRuntimeComponent::ImageResources Frontend()
  {
    return winrt::make<ImageResources>(mVirtualImage->frontend());
  }

  bool Processed() { return mVirtualImage->processed(); }

  void FinishProcessing() { mVirtualImage->finishProcessing(); }

  std::shared_ptr<PB::VirtualImage> Unwrap() { return mVirtualImage; }

private:
  std::shared_ptr<PB::VirtualImage> mVirtualImage;
};
} // namespace winrt::PhotobookRuntimeComponent::implementation
