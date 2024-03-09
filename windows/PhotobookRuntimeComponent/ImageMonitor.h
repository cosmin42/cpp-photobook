#pragma once

#include "ImageMonitor.g.h"

namespace winrt::PhotobookRuntimeComponent::implementation {
struct ImageMonitor : ImageMonitorT<ImageMonitor> {
  ImageMonitor() {}
  ~ImageMonitor() = default;

  void replaceImageMonitorData(
      Windows::Foundation::Collections::IVector<VirtualImagePtr> unstagedImages,
      Windows::Foundation::Collections::IVector<winrt::hstring> roots)
  {
  }

private:
};
} // namespace winrt::PhotobookRuntimeComponent::implementation
