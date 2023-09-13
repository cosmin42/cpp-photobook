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
using namespace Windows::Storage::Pickers;
using namespace Microsoft::UI::Dispatching;

// To learn more about WinUI, the WinUI project structure,
// and more about our project templates, see: http://aka.ms/winui-project-info.

namespace winrt::PhotobookUI::implementation {
TableContentPage::TableContentPage() : mPhotoBook(std::ref(*this))
{
  InitializeComponent();

  mediaListItemsCollection =
      winrt::single_threaded_observable_vector<winrt::hstring>();
}

auto TableContentPage::fireFolderPicker(HWND hWnd) -> winrt::fire_and_forget
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

void TableContentPage::onFinished()
{
  mediaListItemsCollection.Clear();
  auto rootFolders = mPhotoBook.rootPaths();
  for (auto &path : rootFolders)
    mediaListItemsCollection.Append(
        winrt::to_hstring(path.filename().string()));
  this->MediaListView().ItemsSource(mediaListItemsCollection);

  if (!rootFolders.empty()) {
    this->AddMediaButton().VerticalAlignment(VerticalAlignment::Bottom);
  }
  else {
    this->AddMediaButton().VerticalAlignment(VerticalAlignment::Center);
  }
}

void TableContentPage::onStopped() {}

void TableContentPage::onStarted() {}

void TableContentPage::onPaused() {}

void TableContentPage::onResumed() {}

void TableContentPage::onProgressUpdate() {}

void TableContentPage::onError(PB::Error error) {}

void TableContentPage::post(std::function<void()> f)
{
  bool success = MainWindow::sMainthreadDispatcher.TryEnqueue(
      DispatcherQueuePriority::Normal, [f{f}]() { f(); });

  assert(success);
}

} // namespace winrt::PhotobookUI::implementation
