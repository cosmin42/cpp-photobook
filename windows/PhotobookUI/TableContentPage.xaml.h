// Copyright (c) Microsoft Corporation and Contributors.
// Licensed under the MIT License.

#pragma once

#include "TableContentPage.g.h"
#include <pb/PhotoBook.h>
#include <pb/Scheduable.h>

namespace winrt::PhotobookUI::implementation {
class PhotoBookListener final : public PB::GradualControllableListener {
public:
  PhotoBookListener() {}
  ~PhotoBookListener() {}
  void onFinished() override;
  void onStopped() override;
  void onStarted() override;
  void onPaused() override;
  void onResumed() override;

  void onProgressUpdate() override;
  void onError(PB::Error error) override;

  void post(std::function<void()>) override;
};

struct TableContentPage : TableContentPageT<TableContentPage> {
  TableContentPage();
  ~TableContentPage() = default;

  void
  onAddMediaButtonClicked(Windows::Foundation::IInspectable const    &sender,
                          Microsoft::UI::Xaml::RoutedEventArgs const &args);

  void onBackClicked(Windows::Foundation::IInspectable const    &sender,
                     Microsoft::UI::Xaml::RoutedEventArgs const &args);

private:
  winrt::fire_and_forget fireFolderPicker(HWND hWnd);

  PhotoBookListener mListener{};
  PB::PhotoBook     mPhotoBook;

  Windows::Foundation::IAsyncOperation<Windows::Storage::StorageFolder>
      mFolderAsync;
};
} // namespace winrt::PhotobookUI::implementation

namespace winrt::PhotobookUI::factory_implementation {
struct TableContentPage
    : TableContentPageT<TableContentPage, implementation::TableContentPage> {};
} // namespace winrt::PhotobookUI::factory_implementation
