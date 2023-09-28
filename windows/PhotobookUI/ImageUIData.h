#pragma once
#include "ImageUIData.g.h"
#include "ImageUIData.h"

namespace winrt::PhotobookUI::implementation {
struct ImageUIData : ImageUIDataT<ImageUIData> {
  ImageUIData(winrt::hstring fullPath);

  winrt::hstring FullPath();

private:
  winrt::hstring mFullPath;
};
} // namespace winrt::PhotobookUI::implementation
namespace winrt::PhotobookUI::factory_implementation {
struct ImageUIData : ImageUIDataT<ImageUIData, implementation::ImageUIData> {};
} // namespace winrt::PhotobookUI::factory_implementation
