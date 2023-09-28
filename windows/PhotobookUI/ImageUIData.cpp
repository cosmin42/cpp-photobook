// BookstoreViewModel.cpp

// clang-format off
#include "pch.h"
#include "ImageUIData.h"
#if __has_include("ImageUIData.g.cpp")
#include "ImageUIData.g.cpp"
#endif
// clang-format on

namespace winrt::PhotobookUI::implementation {

ImageUIData::ImageUIData(winrt::hstring fullPath) : mFullPath(fullPath) {}

winrt::hstring ImageUIData::FullPath() { return mFullPath; }

} // namespace winrt::PhotobookUI::implementation