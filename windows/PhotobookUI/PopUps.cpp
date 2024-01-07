
// clang-format off
#include "pch.h"
#include "PopUps.h"
// clang-format on

#include <Shobjidl.h>

#include <winrt/Windows.Storage.Pickers.h>

#include <pb/Config.h>
#include <pb/project/Project.h>

namespace winrt::PhotobookUI::implementation {

auto PopUps::fireFolderPicker(HWND                          hWnd,
                              std::function<void(Path)> onSuccess)
    -> winrt::fire_and_forget
{
  Windows::Storage::Pickers::FolderPicker folderPicker;

  auto initializeWithWindow{folderPicker.as<::IInitializeWithWindow>()};
  initializeWithWindow->Initialize(hWnd);

  folderPicker.FileTypeFilter().Append(L"*");
  auto folder{co_await folderPicker.PickSingleFolderAsync()};

  if (folder) {
    onSuccess(winrt::to_string(folder.Path()));
  }
}
} // namespace winrt::PhotobookUI::implementation
