// Copyright (c) Microsoft Corporation and Contributors.
// Licensed under the MIT License.

#pragma once

#include <winrt/Microsoft.Graphics.Canvas.UI.Xaml.h>
#include <winrt/Microsoft.Graphics.Canvas.UI.h>
#include <winrt/Microsoft.Graphics.Canvas.h>

#include "TableContentPage.g.h"
#include <PhotoBookListener.h>
#include <pb/PhotoBook.h>

#include <PopUps.h>

using namespace winrt::Windows::Foundation::Collections;

namespace winrt::PhotobookUI::implementation {

struct TableContentPage : TableContentPageT<TableContentPage> {

  static PB::Path CurrentAppLocation();

  static std::pair<int, int> ScreenSize();

  TableContentPage();
  ~TableContentPage() = default;

  int CanvasMinWidth();
  int CanvasMinHeight();

  int CanvasWidth();
  int CanvasHeight();

  /* Menu Bar */
  void OnExportClicked(Windows::Foundation::IInspectable const    &sender,
                       Microsoft::UI::Xaml::RoutedEventArgs const &args);

  void OnBackClicked(Windows::Foundation::IInspectable const    &sender,
                     Microsoft::UI::Xaml::RoutedEventArgs const &args);

  void OnAboutClicked(Windows::Foundation::IInspectable const    &sender,
                      Microsoft::UI::Xaml::RoutedEventArgs const &args);

  void OnLicenseClicked(Windows::Foundation::IInspectable const    &sender,
                        Microsoft::UI::Xaml::RoutedEventArgs const &args);

  void OnExitClicked(Windows::Foundation::IInspectable const    &sender,
                     Microsoft::UI::Xaml::RoutedEventArgs const &args);

  /* Navigation */
  void OnImportFolderAdded(Windows::Foundation::IInspectable const    &sender,
                           Microsoft::UI::Xaml::RoutedEventArgs const &args);

  void OnImportSelectionChanged(
      [[maybe_unused]] ::winrt::Windows::Foundation::IInspectable const &,
      [[maybe_unused]] ::winrt::Microsoft::UI::Xaml::Controls::
          SelectionChangedEventArgs const &);

  /* Gallery */
  void OnGalleryLeft(Windows::Foundation::IInspectable const    &sender,
                     Microsoft::UI::Xaml::RoutedEventArgs const &args);

  void OnGalleryRight(Windows::Foundation::IInspectable const    &sender,
                      Microsoft::UI::Xaml::RoutedEventArgs const &args);

  void CanvasControlDraw([[maybe_unused]] winrt::Microsoft::Graphics::Canvas::
                             UI::Xaml::CanvasControl const &sender,
                         [[maybe_unused]] winrt::Microsoft::Graphics::Canvas::
                             UI::Xaml::CanvasDrawEventArgs const &args);

  /* Book Lines */

  void OnNavigatedTo(Microsoft::UI::Xaml::Navigation::NavigationEventArgs);

  void OnUnstagedPhotosSelectionChanged(
      [[maybe_unused]] ::winrt::Windows::Foundation::IInspectable const &,
      [[maybe_unused]] ::winrt::Microsoft::UI::Xaml::Controls::
          SelectionChangedEventArgs const &);

  void OnStagedPhotosSelectionChanged(
      [[maybe_unused]] ::winrt::Windows::Foundation::IInspectable const &,
      [[maybe_unused]] ::winrt::Microsoft::UI::Xaml::Controls::
          SelectionChangedEventArgs const &);

  void OnUnstagedPhotosDragStarted(
      Windows::Foundation::IInspectable const                         &sender,
      Microsoft::UI::Xaml::Controls::DragItemsStartingEventArgs const &args);

  void OnDropIntoStagedPhotos(
      [[maybe_unused]] Windows::Foundation::IInspectable const  &sender,
      [[maybe_unused]] Microsoft::UI::Xaml::DragEventArgs const &args);

  void OnDragOverStagedPhotos(
      [[maybe_unused]] Windows::Foundation::IInspectable const  &sender,
      [[maybe_unused]] Microsoft::UI::Xaml::DragEventArgs const &args);

  void OnClickedOutsideList(
      [[maybe_unused]] Windows::Foundation::IInspectable const  &sender,
      [[maybe_unused]] Microsoft::UI::Xaml::Input::TappedRoutedEventArgs const
          &args);

  /* Dialogs - Save */
  void OnContentDialogSaveClicked(
      Windows::Foundation::IInspectable const &sender,
      Microsoft::UI::Xaml::Controls::ContentDialogButtonClickEventArgs const
          &args);

  void OnContentDialogDiscardClicked(
      Windows::Foundation::IInspectable const &sender,
      Microsoft::UI::Xaml::Controls::ContentDialogButtonClickEventArgs const
          &args);

  void OnContentDialogCancelClicked(
      Windows::Foundation::IInspectable const &sender,
      Microsoft::UI::Xaml::Controls::ContentDialogButtonClickEventArgs const
          &args);

  auto ProjectExitDialogDisplay() -> winrt::fire_and_forget;

  /* Dialogs - Export */
  void OnExportContentDialogClicked(
      Windows::Foundation::IInspectable const &sender,
      Microsoft::UI::Xaml::Controls::ContentDialogButtonClickEventArgs const
          &args);

  void OnPaperComboBoxSelected(
      [[maybe_unused]] Windows::Foundation::IInspectable const &sender,
      [[maybe_unused]] Microsoft::UI::Xaml::Controls::
          SelectionChangedEventArgs const &args);

  auto ExportDialogDisplay() -> winrt::fire_and_forget;

  /* Keyboard */

  void OnKeyPressed(Windows::Foundation::IInspectable const &sender,
                    Microsoft::UI::Xaml::Input::KeyRoutedEventArgs const &arg);

  void OnTableContentSizeChanged(
      [[maybe_unused]] Windows::Foundation::IInspectable const         &sender,
      [[maybe_unused]] Microsoft::UI::Xaml::SizeChangedEventArgs const &args);

  auto GenericErrorDialogDisplay() -> winrt::fire_and_forget;
  auto GenericMessageDialogDisplay() -> winrt::fire_and_forget;

  void OnMappingFinished();
  void OnMappingStopped();
  void OnMappingStarted();
  void OnMappingPaused();
  void OnMappingResumed();

  void OnProgressUpdate(int progress, int reference);

  void OnUnstagedImageAdded(PB::Path fullPath, PB::Path mediumPath,
                            PB::Path smallPath, int position);
  void OnAddingUnstagedImagePlaceholder(unsigned size);

  void OnStagedImageAdded(PB::Thumbnails image, int index = -1);

  void OnStagedImageRemoved(std::vector<int> removedIndexes);

  void OnError(PB::Error error);

  void Post(std::function<void()>);

private:
  void UpdateGalleryLabel();

  void PostponeError(std::string message);

  void UpdateCanvasSize();

  // To be moved, geometry
  double PaperToCanvasRatio(
      int width, int height,
      int boundingBoxWidth = PB::Context::CANVAS_MIN_MAX_WIDTH,
      int boundingBoxHeight = PB::Context::CANVAS_MIN_MAX_HEIGHT);

  PhotoBookListener                 mListener;
  PB::PhotoBook<PhotoBookListener>  mPhotoBook;
  IObservableVector<winrt::hstring> mNavigationItemsCollection;
  IObservableVector<ImageUIData>    mUnstagedImageCollection;
  IObservableVector<ImageUIData>    mStagedImageCollection;
  std::vector<PB::Thumbnails>       mDragAndDropSelectedImages;
  PopUps                            mPopups;
  bool                              mExitFlag = false;
  std::pair<int, int> mCanvasSize = {PB::Context::CANVAS_MIN_MAX_WIDTH,
                                     PB::Context::CANVAS_MIN_MAX_HEIGHT};
};
} // namespace winrt::PhotobookUI::implementation

namespace winrt::PhotobookUI::factory_implementation {
struct TableContentPage
    : TableContentPageT<TableContentPage, implementation::TableContentPage> {};
} // namespace winrt::PhotobookUI::factory_implementation
