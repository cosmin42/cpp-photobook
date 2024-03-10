#pragma once

#include "ImageViews.g.h"

#include "ImageMonitor.h"
#include "StagedImages.h"

#include <pb/DataManager.h>

namespace winrt::PhotobookRuntimeComponent::implementation {
struct ImageViews : ImageViewsT<ImageViews> {
  ImageViews(PB::ImageViews &imageViews) : mImageViews(imageViews) {}
  ~ImageViews() = default;

  PhotobookRuntimeComponent::ImageMonitor ImageMonitor()
  {
    return winrt::make<
        winrt::PhotobookRuntimeComponent::implementation::ImageMonitor>(
        mImageViews.imageMonitor());
  }

  PhotobookRuntimeComponent::StagedImages StagedImages()
  {
    return winrt::make<
        winrt::PhotobookRuntimeComponent::implementation::StagedImages>(
        mImageViews.stagedImages());
  }

private:
  PB::ImageViews &mImageViews;
};
} // namespace winrt::PhotobookRuntimeComponent::implementation
