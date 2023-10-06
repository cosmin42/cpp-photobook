#pragma once
#include "ImageUIData.g.h"
#include "ImageUIData.h"

namespace winrt::PhotobookUI::implementation {
struct ImageUIData : ImageUIDataT<ImageUIData> {
  ImageUIData();
  explicit ImageUIData(winrt::hstring fullPath);

  winrt::hstring FullPath();

private:
  static constexpr const char *DEFAULT_IMAGE = "Assets/coffee-cup.png";
  winrt::hstring mFullPath;
};
} // namespace winrt::PhotobookUI::implementation
namespace winrt::PhotobookUI::factory_implementation {
struct ImageUIData : ImageUIDataT<ImageUIData, implementation::ImageUIData> {};
} // namespace winrt::PhotobookUI::factory_implementation
