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

  winrt::hstring fullPath();
  winrt::hstring mediumPath();
  winrt::hstring smallPath();

  void setFullPath(winrt::hstring const fullPath);
  void setMediumPath(winrt::hstring const mediumPath);
  void setSmallPath(winrt::hstring const smallPath);

private:
  PB::ImageResources mImageResource;
};
} // namespace winrt::PhotobookRuntimeComponent::implementation