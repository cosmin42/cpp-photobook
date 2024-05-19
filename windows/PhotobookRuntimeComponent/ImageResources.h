#pragma once

#include "ImageResources.g.h"

#include <pb/image/VirtualImage.h>

namespace winrt::PhotobookRuntimeComponent::implementation {
struct ImageResources : ImageResourcesT<ImageResources> {

  explicit ImageResources(PB::ImageResources imageResources)
      : mImageResource(imageResources)
  {
  }
  ~ImageResources() = default;

  winrt::hstring FullPath();
  winrt::hstring MediumPath();
  winrt::hstring SmallPath();

private:
  PB::ImageResources mImageResource;
};
} // namespace winrt::PhotobookRuntimeComponent::implementation