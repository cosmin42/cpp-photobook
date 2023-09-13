// Copyright (c) Microsoft Corporation and Contributors.
// Licensed under the MIT License.

#pragma once

#include "TableContentPage.g.h"
#include <pb/PhotoBook.h>
#include <pb/Scheduable.h>

using namespace winrt::Windows::Foundation::Collections;
using namespace winrt::Windows::Foundation;

namespace winrt::PhotobookUI::implementation {

struct TableContentPage : TableContentPageT<TableContentPage>,
                          public PB::GradualControllableListener {
  TableContentPage();
  ~TableContentPage() = default;

  void
  onAddMediaButtonClicked(Windows::Foundation::IInspectable const    &sender,
                          Microsoft::UI::Xaml::RoutedEventArgs const &args);

  void onBackClicked(Windows::Foundation::IInspectable const    &sender,
                     Microsoft::UI::Xaml::RoutedEventArgs const &args);

  void onFinished() override;
  void onStopped() override;
  void onStarted() override;
  void onPaused() override;
  void onResumed() override;

  void onProgressUpdate() override;
  void onError(PB::Error error) override;

  void post(std::function<void()>) override;

private:
  auto fireFolderPicker(HWND hWnd) -> winrt::fire_and_forget;

  PB::PhotoBook                                    mPhotoBook;
  IObservableVector<winrt::hstring>                mediaListItemsCollection;
  IAsyncOperation<Windows::Storage::StorageFolder> mFolderAsync;
};
} // namespace winrt::PhotobookUI::implementation

namespace winrt::PhotobookUI::factory_implementation {
struct TableContentPage
    : TableContentPageT<TableContentPage, implementation::TableContentPage> {};
} // namespace winrt::PhotobookUI::factory_implementation
