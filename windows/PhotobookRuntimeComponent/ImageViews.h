#pragma once

#include "ImageViews.g.h"

#include "ImageMonitor.h"
#include "StagedImages.h"

namespace winrt::PhotobookRuntimeComponent::implementation {
struct ImageViews : ImageViewsT<ImageViews> {
  ImageViews() {}
  ~ImageViews() = default;

  ImageMonitor imageMonitor() { return winrt::make<ImageMonitor>(); }
  StagedImages stagedImages() { return winrt::make<StagedImages>(); }

private:
};
} // namespace winrt::PhotobookRuntimeComponent::implementation
