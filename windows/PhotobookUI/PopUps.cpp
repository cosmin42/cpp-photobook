
// clang-format off
#include "pch.h"
#include "PopUps.h"
// clang-format on

#include <Shobjidl.h>

#include <winrt/Windows.Storage.Pickers.h>

namespace winrt::PhotobookUI::implementation {

auto PopUps::fireFolderPicker(HWND                             hWnd,
                              std::function<void(std::string)> onSuccess)
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

auto PopUps::fireSaveFilePicker(
    HWND                                                      hWnd,
    std::function<void(std::variant<std::string, PB::Error>)> onReturn)
    -> winrt::fire_and_forget
{
  Windows::Storage::Pickers::FileSavePicker fileSavePicker;

  auto initializeWithWindow{fileSavePicker.as<::IInitializeWithWindow>()};
  initializeWithWindow->Initialize(hWnd);

  auto plainTextExtensions = winrt::single_threaded_vector<winrt::hstring>();
  plainTextExtensions.Append(winrt::param::hstring(L".photobook"));

  fileSavePicker.SuggestedStartLocation(
      Windows::Storage::Pickers::PickerLocationId::DocumentsLibrary);
  fileSavePicker.FileTypeChoices().Insert(L"Text File", plainTextExtensions);
  fileSavePicker.DefaultFileExtension(L".photobook");
  fileSavePicker.SuggestedFileName(L"Untitled");

  auto filename{co_await fileSavePicker.PickSaveFileAsync()};

  if (filename) {
    onReturn(winrt::to_string(filename.Path()));
  }
  else {
    onReturn(PB::Error() << PB::ErrorCode::CannotSaveFile);
  }
}
} // namespace winrt::PhotobookUI::implementation
