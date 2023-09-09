// Copyright (c) Microsoft Corporation and Contributors.
// Licensed under the MIT License.

#include "pch.h"
#include "TableContentPage.xaml.h"
#if __has_include("TableContentPage.g.cpp")
#include "TableContentPage.g.cpp"
#endif

#include <coroutine>

#include "FirstPage.xaml.h"
#include "MainWindow.xaml.h"
#include <Shobjidl.h>

#include <winrt/Windows.Storage.Pickers.h>
#include <winrt/Windows.UI.Xaml.Interop.h>

#include <Converter.h>
#include <pb/common/Log.h>

using namespace winrt;
using namespace Microsoft::UI::Xaml;
using namespace Windows::Storage;
using namespace Windows::Foundation;
using namespace Windows::Storage::Pickers;

// To learn more about WinUI, the WinUI project structure,
// and more about our project templates, see: http://aka.ms/winui-project-info.

namespace winrt::PhotobookUI::implementation {
TableContentPage::TableContentPage() : mPhotoBook(*this)
{
  InitializeComponent();
}

winrt::fire_and_forget TableContentPage::fireFolderPicker(HWND hWnd)
{
  Windows::Storage::Pickers::FolderPicker folderPicker;

  auto initializeWithWindow{folderPicker.as<::IInitializeWithWindow>()};
  initializeWithWindow->Initialize(hWnd);

  folderPicker.FileTypeFilter().Append(L"*");
  auto folder{co_await folderPicker.PickSingleFolderAsync()};

  mPhotoBook.addMedia(NativePB::Converter()(folder.Path()));
}

void TableContentPage::onAddMediaButtonClicked(IInspectable const &,
                                               RoutedEventArgs const &)
{
  fireFolderPicker(MainWindow::sMainWindowhandle);
}

void TableContentPage::onBackClicked(IInspectable const &,
                                     RoutedEventArgs const &)
{
  Frame().Navigate(winrt::xaml_typename<PhotobookUI::FirstPage>());
}

void TableContentPage::onFinished() {}

void TableContentPage::onStopped() {}

void TableContentPage::onStarted() {}

void TableContentPage::onPaused() {}

void TableContentPage::onResumed() {}

void TableContentPage::onProgressUpdate() {}

void TableContentPage::onError(PB::Error error) {}

void TableContentPage::post(std::function<void()> f)
{

}

} // namespace winrt::PhotobookUI::implementation
