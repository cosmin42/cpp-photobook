// BookstoreViewModel.cpp

// clang-format off
#include "pch.h"
#include "ProjectItem.h"
#if __has_include("ProjectItem.g.cpp")
#include "ProjectItem.g.cpp"
#endif
// clang-format on

namespace winrt::PhotobookUI::implementation {

ProjectItem::ProjectItem(winrt::hstring itemName, winrt::hstring fullPath)
    : mName(itemName), mFullPath(fullPath)
{
}

winrt::hstring ProjectItem::Name() { return mName; }

} // namespace winrt::PhotobookUI::implementation