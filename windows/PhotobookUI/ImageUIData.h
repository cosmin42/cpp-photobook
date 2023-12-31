#pragma once
#include "ImageUIData.g.h"
#include "ImageUIData.h"

namespace winrt::PhotobookUI::implementation {
struct ImageUIData : ImageUIDataT<ImageUIData> {
  ImageUIData();
  explicit ImageUIData(winrt::hstring full, winrt::hstring medium,
                       winrt::hstring smallPath);

  winrt::hstring FullPath();
  winrt::hstring MediumPath();
  winrt::hstring SmallPath();

private:
  winrt::hstring mFullPath;
  winrt::hstring mMediumPath;
  winrt::hstring mSmallPath;
};
} // namespace winrt::PhotobookUI::implementation
namespace winrt::PhotobookUI::factory_implementation {
struct ImageUIData : ImageUIDataT<ImageUIData, implementation::ImageUIData> {};
} // namespace winrt::PhotobookUI::factory_implementation
