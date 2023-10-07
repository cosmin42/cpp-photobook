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
    : mId(itemName), mFullPath(fullPath)
{
}

winrt::hstring ProjectItem::ItemId() { return mId; }

winrt::hstring ProjectItem::FullPath() { return mFullPath; }

} // namespace winrt::PhotobookUI::implementation