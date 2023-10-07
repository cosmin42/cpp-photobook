// BookstoreViewModel.cpp

// clang-format off
#include "pch.h"
#include "ProjectItem.h"
#if __has_include("ProjectItem.g.cpp")
#include "ProjectItem.g.cpp"
#endif
// clang-format on

#include <filesystem>

namespace winrt::PhotobookUI::implementation {

ProjectItem::ProjectItem(winrt::hstring itemName, winrt::hstring fullPath)
    : mId(itemName), mFullPath(fullPath)
{
  std::string           nativeFullPath = winrt::to_string(fullPath);
  std::filesystem::path path(nativeFullPath);

  std::string fileName = path.filename().string();
  auto        index = fileName.find_last_of(".");

  if (index != std::string::npos) {
    mName = winrt::to_hstring(fileName.substr(0, index));
  }
  else {
    mName = winrt::to_hstring(fileName);
  }
};

winrt::hstring ProjectItem::ItemId() { return mId; }

winrt::hstring ProjectItem::FullPath() { return mFullPath; }

winrt::hstring ProjectItem::Name() { return mName; }

} // namespace winrt::PhotobookUI::implementation