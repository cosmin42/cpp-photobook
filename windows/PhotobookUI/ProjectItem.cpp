// BookstoreViewModel.cpp

// clang-format off
#include "pch.h"
#include "ProjectItem.h"
#if __has_include("ProjectItem.g.cpp")
#include "ProjectItem.g.cpp"
#endif
// clang-format on

namespace winrt::PhotobookUI::implementation {

winrt::hstring ProjectItem::name() { return winrt::to_hstring(mName); }

} // namespace winrt::Bookstore::implementation