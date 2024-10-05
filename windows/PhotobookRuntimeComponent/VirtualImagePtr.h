#pragma once

#include "ImageResources.h"
#include "VirtualImagePtr.g.h"

#include "Int32Pair.h"

#include <pb/image/ImageFactory.h>
#include <pb/image/ImageReader.h>

#include <winrt/Microsoft.Graphics.Canvas.h>

namespace winrt::PhotobookRuntimeComponent::implementation {
struct VirtualImagePtr : VirtualImagePtrT<VirtualImagePtr> {

  explicit VirtualImagePtr(PB::GenericImagePtr genericImage)
      : mGenericImage(genericImage)
  {
  }

  ~VirtualImagePtr() = default;

  VirtualImageType Imagetype()
  {
    return (VirtualImageType)mGenericImage->type();
  }

  void GalleryProjection(winrt::array_view<uint8_t> buffer, int portviewWidth,
                         int portviewHeight)
  {
    auto mediumThumbnailPath = mGenericImage->medium();

    auto tmpImage = PB::ImageReader().read(mediumThumbnailPath.string(), false,
                                           {portviewWidth, portviewHeight});

    std::copy_n((uint8_t *)tmpImage->data,
                (uint32_t)(tmpImage->total() * tmpImage->elemSize()),
                buffer.begin());
  }

  PhotobookRuntimeComponent::ImageResources Frontend()
  {
    return winrt::make<ImageResources>(
        mGenericImage->full(), mGenericImage->medium(), mGenericImage->smaLL());
  }

  PB::GenericImagePtr Unwrap() { return mGenericImage; }

private:
  PB::GenericImagePtr mGenericImage = nullptr;
};
} // namespace winrt::PhotobookRuntimeComponent::implementation
