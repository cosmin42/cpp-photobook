// Copyright (c) Microsoft Corporation and Contributors.
// Licensed under the MIT License.

// clang-format off
#include "pch.h"
#include "TableContentPage.xaml.h"
#if __has_include("TableContentPage.g.cpp")
#include "TableContentPage.g.cpp"
#endif
// clang-format on

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

PhotoBookListener::PhotoBookListener(TableContentPage &parent) : mParent(parent)
{
}
void PhotoBookListener::onFinished()
{
  post([this]() { mParent.onFinished(); });
}
void PhotoBookListener::onStopped()
{
  post([this]() { mParent.onStopped(); });
}
void PhotoBookListener::onStarted()
{
  post([this]() { mParent.onStarted(); });
}
void PhotoBookListener::onPaused()
{
  post([this]() { mParent.onPaused(); });
}
void PhotoBookListener::onResumed()
{
  post([this]() { mParent.onResumed(); });
}

void PhotoBookListener::onProgressUpdate()
{
  post([this]() { mParent.onProgressUpdate(); });
}
void PhotoBookListener::onError(PB::Error error)
{
  post([error{error}, this]() { mParent.onError(error); });
}

void PhotoBookListener::post(std::function<void()> f)
{
  mParent.post(std::forward<std::function<void()>>(f));
}

TableContentPage::TableContentPage()
    : mListener(std::ref(*this)), mPhotoBook(mListener)
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

void TableContentPage::onGalleryLeft(
    Windows::Foundation::IInspectable const    &sender,
    Microsoft::UI::Xaml::RoutedEventArgs const &args)
{
  if (mCurrentGalleryIterator) {
    mCurrentGalleryIterator->operator--();
  }
}

void TableContentPage::onGalleryRight(
    Windows::Foundation::IInspectable const    &sender,
    Microsoft::UI::Xaml::RoutedEventArgs const &args)
{
  mCurrentGalleryIterator->operator++();
}

void TableContentPage::onFinished()
{
  mediaListItemsCollection.Clear();
  auto rootFolders = mPhotoBook.rootPaths();
  for (auto &path : rootFolders)
    mediaListItemsCollection.Append(
        winrt::to_hstring(path.filename().string()));
  MediaListView().ItemsSource(mediaListItemsCollection);

  if (!rootFolders.empty()) {
    AddMediaButton().VerticalAlignment(VerticalAlignment::Bottom);
  }
  else {
    AddMediaButton().VerticalAlignment(VerticalAlignment::Center);
  }

  StatusLabelText().Text(winrt::to_hstring("Status: Idle"));
}

void TableContentPage::onFoldersSelectionChanged(
    [[maybe_unused]] ::winrt::Windows::Foundation::IInspectable const &object,
    [[maybe_unused]] ::winrt::Microsoft::UI::Xaml::Controls::
        SelectionChangedEventArgs const &eventArgs)
{
  auto index = MediaListView().SelectedIndex();
  auto mediaMapper = mPhotoBook.mediaMapper(index);
  if (mediaMapper) {
    mCurrentGalleryIterator = mediaMapper->iterator();
  }
  else {
    PB::printError("Media mapper doesn't exist on selection.\n");
  }
}

void TableContentPage::onStopped() {}

void TableContentPage::onStarted() {}

void TableContentPage::onPaused() {}

void TableContentPage::onResumed() {}

void TableContentPage::onProgressUpdate()
{
  StatusLabelText().Text(winrt::to_hstring("Status: In progress..."));
}

void TableContentPage::onError(PB::Error error) {}

void TableContentPage::post(std::function<void()> f)
{
  bool success = MainWindow::sMainthreadDispatcher.TryEnqueue(
      DispatcherQueuePriority::Normal, [f{f}]() { f(); });

  assert(success);
}

} // namespace winrt::PhotobookUI::implementation
