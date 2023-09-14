// Copyright (c) Microsoft Corporation and Contributors.
// Licensed under the MIT License.

#pragma once

#include "TableContentPage.g.h"
#include <PhotoBookListener.h>
#include <pb/PhotoBook.h>
#include <pb/Scheduable.h>

#include <winrt/Microsoft.Graphics.Canvas.UI.Xaml.h>
#include <winrt/Microsoft.Graphics.Canvas.UI.h>
#include <winrt/Microsoft.Graphics.Canvas.h>

using namespace winrt::Windows::Foundation::Collections;

namespace winrt::PhotobookUI::implementation {

struct TableContentPage : TableContentPageT<TableContentPage> {
  TableContentPage();
  ~TableContentPage() = default;

  void
  onAddMediaButtonClicked(Windows::Foundation::IInspectable const    &sender,
                          Microsoft::UI::Xaml::RoutedEventArgs const &args);

  void onBackClicked(Windows::Foundation::IInspectable const    &sender,
                     Microsoft::UI::Xaml::RoutedEventArgs const &args);

  void onGalleryLeft(Windows::Foundation::IInspectable const    &sender,
                     Microsoft::UI::Xaml::RoutedEventArgs const &args);

  void onGalleryRight(Windows::Foundation::IInspectable const    &sender,
                      Microsoft::UI::Xaml::RoutedEventArgs const &args);

  void CanvasControlDraw([[maybe_unused]] winrt::Microsoft::Graphics::Canvas::
                             UI::Xaml::CanvasControl const &sender,
                         [[maybe_unused]] winrt::Microsoft::Graphics::Canvas::
                             UI::Xaml::CanvasDrawEventArgs const &args)
  {
  }

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
  std::optional<
      PB::CircularIterator<PB::MediaMapper<PhotoBookListener>, PB::Path>>
      mCurrentGalleryIterator = std::nullopt;
};
} // namespace winrt::PhotobookUI::implementation

namespace winrt::PhotobookUI::factory_implementation {
struct TableContentPage
    : TableContentPageT<TableContentPage, implementation::TableContentPage> {};
} // namespace winrt::PhotobookUI::factory_implementation
