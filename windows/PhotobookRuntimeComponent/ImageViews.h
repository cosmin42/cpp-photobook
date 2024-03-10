#pragma once

#include "ImageViews.g.h"

#include "ImageMonitor.h"
#include "StagedImages.h"

namespace winrt::PhotobookRuntimeComponent::implementation {
struct ImageViews : ImageViewsT<ImageViews> {
  ImageViews() {}
  ~ImageViews() = default;

  ImageMonitor ImageMonitor() { return winrt::make<ImageMonitor>(); }
  StagedImages StagedImages() { return winrt::make<StagedImages>(); }

private:
};
} // namespace winrt::PhotobookRuntimeComponent::implementation
