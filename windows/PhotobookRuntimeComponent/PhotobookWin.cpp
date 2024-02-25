// BookstoreViewModel.cpp

// clang-format off
#include "pch.h"
#include "PhotobookWin.h"
#include "PhotobookWin.g.cpp"
// clang-format on

#include <filesystem>

#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Storage.h>

using namespace winrt::Windows::Storage;

namespace winrt::PhotobookRuntimeComponent::implementation {

void PhotobookWin::recallMetadata() { mPhotobook->recallMetadata(); }

void PhotobookWin::recallProject(winrt::hstring const name)
{
  auto stlName = winrt::to_string(name);
  mPhotobook->recallProject(stlName);
}

void PhotobookWin::newProject(winrt::hstring const name)
{
  auto stlName = winrt::to_string(name);
  mPhotobook->newProject(stlName);
}
} // namespace winrt::PhotobookRuntimeComponent::implementation
