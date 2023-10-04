// Copyright (c) Microsoft Corporation and Contributors.
// Licensed under the MIT License.

#pragma once

#include <winrt/Microsoft.Graphics.Canvas.UI.Xaml.h>
#include <winrt/Microsoft.Graphics.Canvas.UI.h>
#include <winrt/Microsoft.Graphics.Canvas.h>

#include "TableContentPage.g.h"
#include <PhotoBookListener.h>
#include <pb/PhotoBook.h>

#include <WinrtStorage.h>

using namespace winrt::Windows::Foundation::Collections;

namespace winrt::PhotobookUI::implementation {

struct TableContentPage : TableContentPageT<TableContentPage> {
  TableContentPage();
  ~TableContentPage() = default;

  void
  onAddMediaButtonClicked(Windows::Foundation::IInspectable const    &sender,
                          Microsoft::UI::Xaml::RoutedEventArgs const &args);

  void onExportClicked(Windows::Foundation::IInspectable const    &sender,
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
                             UI::Xaml::CanvasDrawEventArgs const &args);

  void onAddToTableClicked(Windows::Foundation::IInspectable const    &sender,
                           Microsoft::UI::Xaml::RoutedEventArgs const &args);

  void OnNavigatedTo(Microsoft::UI::Xaml::Navigation::NavigationEventArgs);

  void onFoldersSelectionChanged(
      [[maybe_unused]] ::winrt::Windows::Foundation::IInspectable const &,
      [[maybe_unused]] ::winrt::Microsoft::UI::Xaml::Controls::
          SelectionChangedEventArgs const &);

  void onUnstagedListViewSelectionChanged(
      [[maybe_unused]] ::winrt::Windows::Foundation::IInspectable const &,
      [[maybe_unused]] ::winrt::Microsoft::UI::Xaml::Controls::
          SelectionChangedEventArgs const &);

  void onContentDialogSaveClicked(
      Windows::Foundation::IInspectable const &sender,
      Microsoft::UI::Xaml::Controls::ContentDialogButtonClickEventArgs const
          &args);

  void onContentDialogDiscardClicked(
      Windows::Foundation::IInspectable const &sender,
      Microsoft::UI::Xaml::Controls::ContentDialogButtonClickEventArgs const
          &args);

  void onContentDialogCancelClicked(
      Windows::Foundation::IInspectable const &sender,
      Microsoft::UI::Xaml::Controls::ContentDialogButtonClickEventArgs const
          &args);

  void onKeyPressed(Windows::Foundation::IInspectable const &sender,
                    Microsoft::UI::Xaml::Input::KeyRoutedEventArgs const &arg);

  void OnUnstagedListDragStarted(
      Windows::Foundation::IInspectable const                         &sender,
      Microsoft::UI::Xaml::Controls::DragItemsStartingEventArgs const &args);

  void OnDropIntoStagedListView(
      [[maybe_unused]] Windows::Foundation::IInspectable const  &sender,
      [[maybe_unused]] Microsoft::UI::Xaml::DragEventArgs const &args);

  void OnDragOverStagedListView(
      [[maybe_unused]] Windows::Foundation::IInspectable const  &sender,
      [[maybe_unused]] Microsoft::UI::Xaml::DragEventArgs const &args);

  auto projectExitDialogDisplay() -> winrt::fire_and_forget;

  void onFinished();
  void onStopped();
  void onStarted();
  void onPaused();
  void onResumed();

  void onProgressUpdate([[maybe_unused]] int progress,
                        [[maybe_unused]] int reference);

  void onUnstagedImageAdded(PB::Path path);
  void onError(PB::Error error);

  void post(std::function<void()>);

private:
  auto fireFolderPicker(HWND hWnd, std::function<void(std::string)> onSuccess)
      -> winrt::fire_and_forget;

  auto fireSaveFilePicker(
      HWND                                                      hWnd,
      std::function<void(std::variant<std::string, PB::Error>)> onReturn)
      -> winrt::fire_and_forget;

  void updateGalleryLabel();

  PhotoBookListener                                  mListener;
  PB::PhotoBook<PhotoBookListener, PB::WinrtStorage> mPhotoBook;
  IObservableVector<winrt::hstring>                  mMediaListItemsCollection;
  IObservableVector<ImageUIData>                     mUnstagingImageCollection;
  IObservableVector<ImageUIData>                     mStagedImageCollection;
  std::vector<std::string>                           mMediaListNative;
  std::vector<int> mDragAndDropSelectedIndexes;

  std::optional<PB::CircularIterator> mCurrentGalleryIterator = std::nullopt;
};
} // namespace winrt::PhotobookUI::implementation

namespace winrt::PhotobookUI::factory_implementation {
struct TableContentPage
    : TableContentPageT<TableContentPage, implementation::TableContentPage> {};
} // namespace winrt::PhotobookUI::factory_implementation
