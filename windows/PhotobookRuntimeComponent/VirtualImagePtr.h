#pragma once

#include "ImageResources.h"
#include "VirtualImagePtr.g.h"

#include "Int32Pair.h"

#include <pb/image/ImageFactory.h>
#include <pb/image/ImageReader.h>

#include <winrt/Microsoft.Graphics.Canvas.h>

namespace winrt::PhotobookRuntimeComponent::implementation {
struct VirtualImagePtr : VirtualImagePtrT<VirtualImagePtr> {

  explicit VirtualImagePtr(PB::GenericImagePtr genericImage,
                           std::string         thumbnailsLocation)
      : mGenericImage(genericImage), mThumbnailsLocation(thumbnailsLocation)
  {
  }

  ~VirtualImagePtr() = default;

  VirtualImageType Imagetype()
  {
    return (VirtualImageType)mGenericImage->type();
  }

  void GalleryProjection(winrt::array_view<uint8_t> buffer, int portviewWidth,
                         int portviewHeight, winrt::hstring thumbnailsPath)
  {
    auto nativeThumbnailsPath = Path(winrt::to_string(thumbnailsPath));
    auto mediumThumbnailPath = nativeThumbnailsPath / mGenericImage->medium();

    auto tmpImage = PB::ImageReader().read(mediumThumbnailPath.string(), false,
                                           {portviewWidth, portviewHeight});

    std::copy_n((uint8_t *)tmpImage->data,
                (uint32_t)(tmpImage->total() * tmpImage->elemSize()),
                buffer.begin());
  }

  PhotobookRuntimeComponent::ImageResources Frontend()
  {
    auto thumbnailsLocationPath = Path(mThumbnailsLocation);
    return winrt::make<ImageResources>(
        thumbnailsLocationPath / mGenericImage->full(),
        thumbnailsLocationPath / mGenericImage->medium(),
        thumbnailsLocationPath / mGenericImage->smaLL());
  }

  PB::GenericImagePtr Unwrap() { return mGenericImage; }

private:
  PB::GenericImagePtr mGenericImage = nullptr;
  std::string         mThumbnailsLocation;
};
} // namespace winrt::PhotobookRuntimeComponent::implementation
