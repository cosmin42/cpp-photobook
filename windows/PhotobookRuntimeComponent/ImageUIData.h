#pragma once

#include "ImageUIData.g.h"

namespace winrt::PhotobookRuntimeComponent::implementation {
struct ImageUIData : ImageUIDataT<ImageUIData> {
  ImageUIData();
  explicit ImageUIData(winrt::hstring key, winrt::hstring full,
                       winrt::hstring medium, winrt::hstring smallPath,
                       bool processed);

  winrt::hstring KeyPath();
  winrt::hstring FullPath();
  winrt::hstring MediumPath();
  winrt::hstring SmallPath();
  bool           Processed();

private:
  winrt::hstring mKey;
  winrt::hstring mFullPath;
  winrt::hstring mMediumPath;
  winrt::hstring mSmallPath;
  bool           mProcessed = false;
};
} // namespace winrt::PhotobookRuntimeComponent::implementation
namespace winrt::PhotobookRuntimeComponent::factory_implementation {
struct ImageUIData : ImageUIDataT<ImageUIData, implementation::ImageUIData> {};
} // namespace winrt::PhotobookRuntimeComponent::factory_implementation
