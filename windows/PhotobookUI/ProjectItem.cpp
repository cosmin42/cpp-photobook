// BookstoreViewModel.cpp

// clang-format off
#include "pch.h"
#include "ProjectItem.h"
#if __has_include("ProjectItem.g.cpp")
#include "ProjectItem.g.cpp"
#endif
// clang-format on

namespace winrt::PhotobookUI::implementation {

ProjectItem::ProjectItem(winrt::hstring itemName) : mName(itemName) {}

winrt::hstring ProjectItem::Name() { return mName; }

} // namespace winrt::PhotobookUI::implementation