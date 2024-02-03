// Copyright (c) Microsoft Corporation and Contributors.
// Licensed under the MIT License.

#pragma once

#include <unordered_set>

#include <winrt/Microsoft.Graphics.Canvas.UI.Xaml.h>
#include <winrt/Microsoft.Graphics.Canvas.UI.h>
#include <winrt/Microsoft.Graphics.Canvas.h>

#include "TableContentPage.g.h"
#include <PhotobookTableListener.h>
#include <pb/PhotoBook.h>

#include <PopUps.h>

using namespace winrt::Windows::Foundation::Collections;

namespace winrt::PhotobookUI::implementation {

enum class DragSource { None, Unstaged, Staged };

struct UISelectionIndex {
  std::optional<unsigned> importListIndex;
  std::vector<unsigned>   unstagedLineIndex;
  std::vector<unsigned>   stagedPhotoIndex;
};

struct TableContentPage : TableContentPageT<TableContentPage>,
                          public PB::StagedImagesListener,
                          public PB::ImageMonitorListener {

  static std::pair<int, int> ScreenSize();

  TableContentPage();
  ~TableContentPage();

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

  void OnTipsClicked(Windows::Foundation::IInspectable const    &sender,
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

  void OnPreferencesClicked(Windows::Foundation::IInspectable const    &sender,
                            Microsoft::UI::Xaml::RoutedEventArgs const &args);

  /* Navigation */
  void OnImportFolderAdded(Windows::Foundation::IInspectable const    &sender,
                           Microsoft::UI::Xaml::RoutedEventArgs const &args);

  void OnImportFolderRemoved(Windows::Foundation::IInspectable const    &sender,
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

  void OnStagedDragItemsStarting(
      [[maybe_unused]] ::winrt::Windows::Foundation::IInspectable const &,
      [[maybe_unused]] ::winrt::Microsoft::UI::Xaml::Controls::
          DragItemsStartingEventArgs const &);

  void OnStagedDragItemsCompleted(
      [[maybe_unused]] ::winrt::Windows::Foundation::IInspectable const &,
      [[maybe_unused]] ::winrt::Microsoft::UI::Xaml::Controls::
          DragItemsCompletedEventArgs const &args);

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
  void OnContentDialogDiscardClicked(
      Windows::Foundation::IInspectable const &sender,
      Microsoft::UI::Xaml::Controls::ContentDialogButtonClickEventArgs const
          &args);

  void OnContentDialogCancelClicked(
      Windows::Foundation::IInspectable const &sender,
      Microsoft::UI::Xaml::Controls::ContentDialogButtonClickEventArgs const
          &args);

  void OnRenameProjectDialogRename(
      [[maybe_unused]] Windows::Foundation::IInspectable const &sender,
      [[maybe_unused]] Microsoft::UI::Xaml::Controls::
          ContentDialogButtonClickEventArgs const &args);

  void OnRenameProjectDialogCancel(
      [[maybe_unused]] Windows::Foundation::IInspectable const &sender,
      [[maybe_unused]] Microsoft::UI::Xaml::Controls::
          ContentDialogButtonClickEventArgs const &args);

  auto RenameProjectDialogDisplay() -> winrt::fire_and_forget;

  /* Dialogs - Export */
  void OnExportContentDialogClicked(
      Windows::Foundation::IInspectable const &sender,
      Microsoft::UI::Xaml::Controls::ContentDialogButtonClickEventArgs const
          &args);

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

  void OnThumbnailsProcessingFinished(Path rootPath);

  void OnMappingFinished(Path path);
  void OnMappingAborted(Path path);
  void OnMappingStarted(Path path);

  void OnProjectRenamed();

  void onProgressUpdate(PB::ProgressInfo definedProgress,
                        PB::ProgressInfo undefinedProgress);

  void OnExportProgressUpdate(int progress, int reference);

  void OnExportFinished();

  void OnAddingUnstagedImagePlaceholder(unsigned size);

  void OnStagedImageAdded(std::vector<std::shared_ptr<PB::VirtualImage>> photos,
                          int index = -1);

  void OnStagedImageRemoved(std::vector<unsigned> removedIndexes);

  void OnError(PBDev::Error error);

  void Post(std::function<void()>);

  void onPicturesAdded(int index, int size) override;
  void onPictureRemoved(std::vector<unsigned> index) override;

  void onImportFolderAdded() override;
  void onRefresh() override;
  void onImportFolderRemoved(unsigned index) override;
  void onCleared() override;

  void UpdateUnstagedImage(int row, int index);

  void UpdateUnstagedLine();

private:
  UISelectionIndex SelectionIndex();
  bool             StagedLineEmpty();
  bool             UnstagedLintEmpty();

  void UpdateGalleryLabel();

  void UpdateUnstagedPhotoLine();
  void UpdateGallery();

  void PostponeError(std::string message);

  void UpdateCanvasSize();

  void UpdateUnstagedImagesView(int index);

  void OnStagedImageCollectionChanged(
      [[maybe_unused]] IObservableVector<ImageUIData> const    &sender,
      winrt::Windows::Foundation::Collections::CollectionChange changeType,
      int                                                       changedIndex);

  // To be moved, geometry
  double PaperToCanvasRatio(
      int width, int height,
      int boundingBoxWidth = PB::Context::CANVAS_MIN_MAX_WIDTH,
      int boundingBoxHeight = PB::Context::CANVAS_MIN_MAX_HEIGHT);

  void UpdateStatusBar(PB::ProgressInfo defined, PB::ProgressInfo undefined);

  void LoadImages();

  PhotobookTableListener                         mListener;
  std::shared_ptr<PB::Photobook>                 mPhotoBook;
  IObservableVector<winrt::hstring>              mNavigationItemsCollection;
  IObservableVector<ImageUIData>                 mUnstagedImageCollection;
  IObservableVector<ImageUIData>                 mStagedImageCollection;
  std::vector<std::shared_ptr<PB::VirtualImage>> mDragAndDropSelectedImages;
  PopUps                                         mPopups;
  bool                                           mExitFlag = false;
  bool                                           mNewProjectFlag = false;
  std::unordered_set<Path>                       mLoadedFinishedImportFolders;
  DragSource                                     mDragSource = DragSource::None;
  bool mLinesExclusiveSelection = false;

  std::unordered_set<Path> mImportedDirectories;
};
} // namespace winrt::PhotobookUI::implementation

namespace winrt::PhotobookUI::factory_implementation {
struct TableContentPage
    : TableContentPageT<TableContentPage, implementation::TableContentPage> {};
} // namespace winrt::PhotobookUI::factory_implementation
