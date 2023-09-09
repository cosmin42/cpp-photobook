// Copyright (c) Microsoft Corporation and Contributors.
// Licensed under the MIT License.

#include "pch.h"
#include "TableContentPage.xaml.h"
#if __has_include("TableContentPage.g.cpp")
#include "TableContentPage.g.cpp"
#endif

#include "FirstPage.xaml.h"

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

void TableContentPage::onAddMediaButtonClicked(IInspectable const &,
                                               RoutedEventArgs const &)
{
  FolderPicker folderPicker;
  folderPicker.SuggestedStartLocation(PickerLocationId::ComputerFolder);
  folderPicker.FileTypeFilter().Append(L"*");

  Windows::Foundation::IAsyncOperation<Windows::Storage::StorageFolder> folderAsync =
      folderPicker.PickSingleFolderAsync();

  folderAsync.Completed(
      [this](Windows::Foundation::IAsyncOperation<
                 Windows::Storage::StorageFolder> const &asyncOp,
             Windows::Foundation::AsyncStatus const      asyncStatus) {
        if (asyncStatus == Windows::Foundation::AsyncStatus::Completed) {
      StorageFolder folder = asyncOp.GetResults();
          mPhotoBook.addMedia(NativePB::Converter()(folder.Path()));
    }
    else {
          PB::printDebug("Folder Not selected");
    }
  });
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

} // namespace winrt::PhotobookUI::implementation
