// Copyright (c) Microsoft Corporation and Contributors.
// Licensed under the MIT License.

#pragma once

#include "TableContentPage.g.h"
#include <pb/PhotoBook.h>
#include <pb/Scheduable.h>

using namespace winrt::Windows::Foundation::Collections;

namespace winrt::PhotobookUI::implementation {

struct TableContentPage;

class PhotoBookListener {
public:
  explicit PhotoBookListener(TableContentPage &parent);
  void onFinished();
  void onStopped();
  void onStarted();
  void onPaused();
  void onResumed();

  void onProgressUpdate();
  void onError(PB::Error error);

  void post(std::function<void()> f);

private:
  TableContentPage &mParent;
};

struct TableContentPage : TableContentPageT<TableContentPage> {
  TableContentPage();
  ~TableContentPage() = default;

  void
  onAddMediaButtonClicked(Windows::Foundation::IInspectable const    &sender,
                          Microsoft::UI::Xaml::RoutedEventArgs const &args);

  void onBackClicked(Windows::Foundation::IInspectable const    &sender,
                     Microsoft::UI::Xaml::RoutedEventArgs const &args);

  void onFoldersSelectionChanged(
      [[maybe_unused]] ::winrt::Windows::Foundation::IInspectable const &,
      [[maybe_unused]] ::winrt::Microsoft::UI::Xaml::Controls::
          SelectionChangedEventArgs const &);

  void onFinished();
  void onStopped();
  void onStarted();
  void onPaused();
  void onResumed();

  void onProgressUpdate();
  void onError(PB::Error error);

  void post(std::function<void()>);

private:
  auto              fireFolderPicker(HWND hWnd) -> winrt::fire_and_forget;
  PhotoBookListener mListener;
  PB::PhotoBook<PhotoBookListener>  mPhotoBook;
  IObservableVector<winrt::hstring> mediaListItemsCollection;
  winrt::Windows::Foundation::IAsyncOperation<Windows::Storage::StorageFolder>
      mFolderAsync;
};
} // namespace winrt::PhotobookUI::implementation

namespace winrt::PhotobookUI::factory_implementation {
struct TableContentPage
    : TableContentPageT<TableContentPage, implementation::TableContentPage> {};
} // namespace winrt::PhotobookUI::factory_implementation
