// Copyright (c) Microsoft Corporation and Contributors.
// Licensed under the MIT License.

#pragma once

#include <unordered_set>

#include <winrt/Microsoft.Graphics.Canvas.UI.Xaml.h>
#include <winrt/Microsoft.Graphics.Canvas.UI.h>
#include <winrt/Microsoft.Graphics.Canvas.h>

#include "TableContentPage.g.h"
#include <PhotobookTableListener.h>
#include <pb/Photobook.h>

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

  void OnSaveClicked(Windows::Foundation::IInspectable const    &sender,
                     Microsoft::UI::Xaml::RoutedEventArgs const &args);

  void OnSaveAsClicked(Windows::Foundation::IInspectable const    &sender,
                       Microsoft::UI::Xaml::RoutedEventArgs const &args);

  void OnNewClicked(Windows::Foundation::IInspectable const    &sender,
                    Microsoft::UI::Xaml::RoutedEventArgs const &args);

  void OnUndoClicked(Windows::Foundation::IInspectable const    &sender,
                     Microsoft::UI::Xaml::RoutedEventArgs const &args);

  void OnRedoClicked(Windows::Foundation::IInspectable const    &sender,
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

  void OnCanvasDraw([[maybe_unused]] winrt::Microsoft::Graphics::Canvas::UI::
                        Xaml::CanvasControl const &sender,
                    [[maybe_unused]] winrt::Microsoft::Graphics::Canvas::UI::
                        Xaml::CanvasDrawEventArgs const &args);

  void Left();
  void Right();

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
      [[maybe_unused]] Windows::Foundation::IInspectable const &sender,
      [[maybe_unused]] Microsoft::UI::Xaml::Input::TappedRoutedEventArgs const
          &args);

  void UnstagedSelectAllInvoked(
      winrt::Microsoft::UI::Xaml::Input::KeyboardAccelerator const,
      winrt::Microsoft::UI::Xaml::Input::
          KeyboardAcceleratorInvokedEventArgs const);

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

  /* #18 */
  void OnKeyDown(Windows::Foundation::IInspectable const              &sender,
                 Microsoft::UI::Xaml::Input::KeyRoutedEventArgs const &arg);

  void
  OnKeyDown(winrt::Microsoft::UI::Xaml::Input::KeyRoutedEventArgs const arg)
  {
    if (arg.Key() == Windows::System::VirtualKey::S &&
        arg.Key() == Windows::System::VirtualKey::LeftControl) {
    }
  }

  void OnTableContentSizeChanged(
      [[maybe_unused]] Windows::Foundation::IInspectable const         &sender,
      [[maybe_unused]] Microsoft::UI::Xaml::SizeChangedEventArgs const &args);

  auto GenericErrorDialogDisplay() -> winrt::fire_and_forget;
  auto GenericMessageDialogDisplay() -> winrt::fire_and_forget;

  void OnThumbnailsProcessingFinished(PB::Path rootPath);
  void OnMappingFinished(PB::Path rootPath);
  void OnMappingStopped();
  void OnMappingStarted();
  void OnMappingPaused();
  void OnMappingResumed();

  void OnProgressUpdate(PB::Path rootPath, int progress, int reference);

  void OnUnstagedImageAdded(PB::Path rootPath, PB::Path fullPath,
                            PB::Path mediumPath, PB::Path smallPath,
                            int position);
  void OnAddingUnstagedImagePlaceholder(unsigned size);

  void OnStagedImageAdded(std::vector<PB::Thumbnails> photos, int index = -1);

  void OnStagedImageRemoved(std::vector<int> removedIndexes);

  void OnError(PB::Error error);

  void Post(std::function<void()>);

private:
  void UpdateGalleryLabel();

  void PostponeError(std::string message);

  void UpdateCanvasSize();

  void UpdateUnstagedImagesView(int index);

  // To be moved, geometry
  double PaperToCanvasRatio(
      int width, int height,
      int boundingBoxWidth = PB::Context::CANVAS_MIN_MAX_WIDTH,
      int boundingBoxHeight = PB::Context::CANVAS_MIN_MAX_HEIGHT);

  PhotobookTableListener            mListener;
  PB::Photobook                     mPhotoBook;
  IObservableVector<winrt::hstring> mNavigationItemsCollection;
  IObservableVector<ImageUIData>    mUnstagedImageCollection;
  IObservableVector<ImageUIData>    mStagedImageCollection;
  std::vector<PB::Thumbnails>       mDragAndDropSelectedImages;
  PopUps                            mPopups;
  bool                              mExitFlag = false;
  std::unordered_set<PB::Path>      mLoadedFinishedImportFolders;
  std::unordered_set<PB::Path>      mStagedImages;
};
} // namespace winrt::PhotobookUI::implementation

namespace winrt::PhotobookUI::factory_implementation {
struct TableContentPage
    : TableContentPageT<TableContentPage, implementation::TableContentPage> {};
} // namespace winrt::PhotobookUI::factory_implementation
