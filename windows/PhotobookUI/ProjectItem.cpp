// BookstoreViewModel.cpp

// clang-format off
#include "pch.h"
#include "ProjectItem.h"
#if __has_include("ProjectItem.g.cpp")
#include "ProjectItem.g.cpp"
#endif
// clang-format on

#include <filesystem>

#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Storage.h>

using namespace winrt::Windows::Storage;

namespace winrt::PhotobookUI::implementation {

ProjectItem::ProjectItem(winrt::hstring id, winrt::hstring fullPath, winrt::hstring name)
    : mId(id), mFullPath(fullPath), mName(name)
{
};

winrt::hstring ProjectItem::ItemId() { return mId; }

winrt::hstring ProjectItem::FullPath() { return mFullPath; }

winrt::hstring ProjectItem::Name() { return mName; }

} // namespace winrt::PhotobookUI::implementation