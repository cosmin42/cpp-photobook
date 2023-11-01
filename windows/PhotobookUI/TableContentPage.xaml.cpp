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

#include "Dashboard.xaml.h"
#include "MainWindow.xaml.h"

#include <microsoft.ui.xaml.window.h>
#include <winrt/Windows.ApplicationModel.Core.h>
#include <winrt/Windows.Storage.h>
#include <winrt/Windows.UI.Core.h>
#include <winrt/Windows.UI.ViewManagement.h>
#include <winrt/Windows.UI.Xaml.Interop.h>

#include <pb/PaperSettings.h>
#include <pb/SerializationStrategy.h>
#include <pb/common/Log.h>

using namespace winrt;
using namespace Microsoft::UI::Xaml;
using namespace Windows::Foundation;
using namespace Windows::Storage;
using namespace Windows::Storage::Pickers;
using namespace Microsoft::UI::Dispatching;
using namespace Microsoft::Graphics::Canvas;

// To learn more about WinUI, the WinUI project structure,
// and more about our project templates, see: http://aka.ms/winui-project-info.

namespace winrt::PhotobookUI::implementation {

PB::Path TableContentPage::CurrentAppLocation()
{
  StorageFolder folder = ApplicationData::Current().LocalFolder();

  return PB::Path(winrt::to_string(folder.Path()));
}

std::pair<int, int> TableContentPage::ScreenSize()
{
  RECT windowSize;

  if (SystemParametersInfoA(SPI_GETWORKAREA, 0, &windowSize, 0)) {
    int windowWidth = windowSize.right - windowSize.left;
    int windowHeight = windowSize.bottom - windowSize.top;
    return {windowWidth, windowHeight};
  }
  return {0, 0};
}

TableContentPage::TableContentPage()
    : mListener(std::ref(*this)),
      mPhotoBook(mListener, CurrentAppLocation(), ScreenSize())
{
  mNavigationItemsCollection =
      winrt::single_threaded_observable_vector<winrt::hstring>();
  mUnstagedImageCollection =
      winrt::single_threaded_observable_vector<ImageUIData>();
  mStagedImageCollection =
      winrt::single_threaded_observable_vector<ImageUIData>();
  InitializeComponent();

  UnstagedListView().ItemsSource(mUnstagedImageCollection);
  StagedListView().ItemsSource(mStagedImageCollection);

  KeyUp([this](Windows::Foundation::IInspectable const              &sender,
               Microsoft::UI::Xaml::Input::KeyRoutedEventArgs const &arg) {
    OnKeyPressed(sender, arg);
  });

  mStagedImageCollection.VectorChanged(
      [this](IObservableVector<ImageUIData> const &sender,
             IVectorChangedEventArgs const        &args) {
        if (mDragSource != DragSource::Staged) {
          return;
        }
        PB::printDebug(
            "Reorder changed index: %d %s\n", args.Index(),
            std::string(magic_enum::enum_name(args.CollectionChange()))
                .c_str());
        auto changeType = args.CollectionChange();
        if (changeType == winrt::Windows::Foundation::Collections::
                              CollectionChange::ItemInserted) {
          auto image = sender.GetAt(args.Index());
          auto fullPath = winrt::to_string(image.FullPath());
          auto mediumPath = winrt::to_string(image.MediumPath());
          auto smallPath = winrt::to_string(image.SmallPath());
          mPhotoBook.addStagedPhoto(
              {PB::Thumbnails(PB::Path(fullPath), PB::Path(mediumPath),
                              PB::Path(smallPath))},
              args.Index());
        }
        else if (changeType == winrt::Windows::Foundation::Collections::
                                   CollectionChange::ItemRemoved) {

          mPhotoBook.removeStagedPhoto(args.Index());
        }
        else if (changeType == winrt::Windows::Foundation::Collections::
                                   CollectionChange::ItemChanged) {
          PB::basicAssert(false);
        }
        else {
          PB::basicAssert(false);
        }
      });

  mUnstagedImageCollection.VectorChanged(
      [this]([[maybe_unused]] IObservableVector<ImageUIData> const &sender,
             [[maybe_unused]] IVectorChangedEventArgs const        &args) {
        if (sender.Size() == 0) {
          AddMediaButton().VerticalAlignment(VerticalAlignment::Center);
          RemoveMediaButton().Visibility(
              winrt::Microsoft::UI::Xaml::Visibility::Collapsed);
        }
        else {
          AddMediaButton().VerticalAlignment(VerticalAlignment::Bottom);
          RemoveMediaButton().VerticalAlignment(VerticalAlignment::Bottom);
          RemoveMediaButton().Visibility(
              winrt::Microsoft::UI::Xaml::Visibility::Visible);
        }
      });
}

void TableContentPage::OnKeyDown(
    [[maybe_unused]] Windows::Foundation::IInspectable const &sender,
    [[maybe_unused]] Microsoft::UI::Xaml::Input::KeyRoutedEventArgs const &arg)
{
}

double TableContentPage::PaperToCanvasRatio(int width, int height,
                                            int boundingBoxWidth,
                                            int boundingBoxHeight)
{
  PB::basicAssert(boundingBoxWidth > 0);
  PB::basicAssert(boundingBoxHeight > 0);
  double widthRatio = (double)width / (double)boundingBoxWidth;
  double heightRatio = (double)height / (double)boundingBoxHeight;

  double maxRatio = std::max<double>(widthRatio, heightRatio);

  return maxRatio;
}

int TableContentPage::CanvasMinWidth()
{
  auto paperSettings = mPhotoBook.paperSettings();
  PB::basicAssert(mPhotoBook.paperSettings().ppi > 0);

  double ratio = PaperToCanvasRatio(paperSettings.width, paperSettings.height);

  if (ratio > 1) {
    return (int)floor((double)paperSettings.width / ratio);
  }
  else {
    return paperSettings.width;
  }
}

int TableContentPage::CanvasMinHeight()
{
  auto paperSettings = mPhotoBook.paperSettings();
  PB::basicAssert(mPhotoBook.paperSettings().ppi > 0);

  double ratio = PaperToCanvasRatio(paperSettings.width, paperSettings.height);

  if (ratio > 1) {
    return (int)floor((double)paperSettings.height / ratio);
  }
  else {
    return paperSettings.height;
  }
}

void TableContentPage::OnImportFolderAdded(IInspectable const &,
                                           RoutedEventArgs const &)
{
  mPopups.fireFolderPicker(
      MainWindow::sMainWindowhandle,
      [this](PB::Path path) { mPhotoBook.addImportFolder(path); });
}

void TableContentPage::OnImportFolderRemoved(IInspectable const &,
                                             RoutedEventArgs const &)
{
  auto selectedIndex = MediaListView().SelectedIndex();
  if (selectedIndex < 0) {
    return;
  }

  StagedListView().DeselectRange(Microsoft::UI::Xaml::Data::ItemIndexRange(
      0, mStagedImageCollection.Size()));

  for (int i = 0; i < (int)mStagedImageCollection.Size(); ++i) {
    PB::Path fullPath =
        winrt::to_string(mStagedImageCollection.GetAt(i).FullPath());
    if (mPhotoBook.imageSupport().fullPathRow(fullPath) == selectedIndex) {
      mPhotoBook.removeStagedPhoto(i);
      mStagedImageCollection.RemoveAt(i);
      i--;
    }
  }
  auto importFoldersSize = MediaListView().Items().Size();
  if (MediaListView().SelectedIndex() > -1) {
    MediaListView().DeselectRange(Microsoft::UI::Xaml::Data::ItemIndexRange(
        0, mNavigationItemsCollection.Size()));
  }
  if (importFoldersSize == 1) {
    mUnstagedImageCollection.Clear();
  }
  else {
    // TODO: Optimize this.
    for (auto i = 0; i < (int)importFoldersSize; ++i) {
      if (i != selectedIndex) {
        MediaListView().SelectedIndex(i);
      }
    }
  }
  mNavigationItemsCollection.RemoveAt(selectedIndex);

  mPhotoBook.imageSupport().removeGroup((int)selectedIndex);

  GalleryCanvas().Invalidate();
}

auto TableContentPage::ProjectExitDialogDisplay() -> winrt::fire_and_forget
{
  co_await ProjectExitDialog().ShowAsync();
}

auto TableContentPage::ExportDialogDisplay() -> winrt::fire_and_forget
{
  co_await ExportContentDialog().ShowAsync();
}

auto TableContentPage::GenericErrorDialogDisplay() -> winrt::fire_and_forget
{
  co_await GenericErrorDialog().ShowAsync();
}

auto TableContentPage::GenericMessageDialogDisplay() -> winrt::fire_and_forget
{
  co_await GnericMessage().ShowAsync();
}

void TableContentPage::OnBackClicked(IInspectable const &,
                                     RoutedEventArgs const &)
{
  ProjectExitDialogDisplay();
}

void TableContentPage::OnAboutClicked(
    [[maybe_unused]] Windows::Foundation::IInspectable const    &sender,
    [[maybe_unused]] Microsoft::UI::Xaml::RoutedEventArgs const &args)
{
  GenericMessageTextBlock().Text(
      winrt::to_hstring("Version: " + std::string(PB::Context::VERSION)));
  GenericMessageDialogDisplay();
}

void TableContentPage::OnLicenseClicked(
    [[maybe_unused]] Windows::Foundation::IInspectable const    &sender,
    [[maybe_unused]] Microsoft::UI::Xaml::RoutedEventArgs const &args)
{
  auto installFolder =
      Windows::ApplicationModel::Package::Current().InstalledLocation();

  PB::Path installedLocation = winrt::to_string(installFolder.Path());

  auto licensePath = installedLocation / "Assets" / "LICENSE";

  std::ifstream licenseFile(licensePath.string());

  std::string licenseText;
  std::string line;

  while (getline(licenseFile, line)) {

    licenseText += line + "\n";
  }

  licenseFile.close();

  GenericMessageTextBlock().Text(winrt::to_hstring(licenseText));
  GenericMessageDialogDisplay();
}

void TableContentPage::OnTipsClicked(
    [[maybe_unused]] Windows::Foundation::IInspectable const    &sender,
    [[maybe_unused]] Microsoft::UI::Xaml::RoutedEventArgs const &args)
{
}

void TableContentPage::OnExitClicked(
    [[maybe_unused]] Windows::Foundation::IInspectable const    &sender,
    [[maybe_unused]] Microsoft::UI::Xaml::RoutedEventArgs const &args)
{
  ProjectExitDialogDisplay();
  mExitFlag = true;
}

void TableContentPage::OnSaveClicked(
    [[maybe_unused]] Windows::Foundation::IInspectable const    &sender,
    [[maybe_unused]] Microsoft::UI::Xaml::RoutedEventArgs const &args)
{
  if (!mPhotoBook.projectDefaultSaved()) {
    mPhotoBook.savePhotobook();
  }
  else {
    mPopups.fireSaveFilePicker(
        MainWindow::sMainWindowhandle,
        [this](std::variant<std::string, PB::Error> result) {
          if (std::holds_alternative<std::string>(result)) {
            auto &newName = std::get<std::string>(result);
            mPhotoBook.savePhotobook(newName);
          }
          else {
            OnError(std::get<PB::Error>(result));
          }
        });
  }
}

void TableContentPage::OnSaveAsClicked(
    [[maybe_unused]] Windows::Foundation::IInspectable const    &sender,
    [[maybe_unused]] Microsoft::UI::Xaml::RoutedEventArgs const &args)
{
  mPopups.fireSaveFilePicker(
      MainWindow::sMainWindowhandle,
      [this](std::variant<std::string, PB::Error> result) {
        if (std::holds_alternative<std::string>(result)) {
          auto &newName = std::get<std::string>(result);
          mPhotoBook.savePhotobook(newName);
        }
        else {
          OnError(std::get<PB::Error>(result));
        }
      });
}

void TableContentPage::OnNewClicked(
    [[maybe_unused]] Windows::Foundation::IInspectable const    &sender,
    [[maybe_unused]] Microsoft::UI::Xaml::RoutedEventArgs const &args)
{
}

void TableContentPage::OnUndoClicked(
    [[maybe_unused]] Windows::Foundation::IInspectable const    &sender,
    [[maybe_unused]] Microsoft::UI::Xaml::RoutedEventArgs const &args)
{
}

void TableContentPage::OnRedoClicked(
    [[maybe_unused]] Windows::Foundation::IInspectable const    &sender,
    [[maybe_unused]] Microsoft::UI::Xaml::RoutedEventArgs const &args)
{
}

void TableContentPage::OnPreferencesClicked(
    [[maybe_unused]] Windows::Foundation::IInspectable const    &sender,
    [[maybe_unused]] Microsoft::UI::Xaml::RoutedEventArgs const &args)
{
}

void TableContentPage::OnKeyPressed(
    [[maybe_unused]] Windows::Foundation::IInspectable const &sender,
    Microsoft::UI::Xaml::Input::KeyRoutedEventArgs const     &arg)
{
  auto selected = mPhotoBook.gallery().selectedItem();
  if (!selected) {
    return;
  }
  Windows::System::VirtualKey key = arg.Key();

  switch (key) {
  case Windows::System::VirtualKey::Delete: {
    auto             ranges = StagedListView().SelectedRanges();
    std::vector<int> selectedIndexes;
    for (auto range : ranges) {
      for (auto i = range.FirstIndex(); i <= range.LastIndex(); ++i) {
        selectedIndexes.push_back(i);
      }
    }

    if (selectedIndexes.size() > 0) {
      mPhotoBook.removeStagedPhoto(selectedIndexes);
      OnStagedImageRemoved(selectedIndexes);
    }
  }
  default: {
  }
  }
}

void TableContentPage::OnUnstagedPhotosDragStarted(
    [[maybe_unused]] Windows::Foundation::IInspectable const &sender,
    [[maybe_unused]] Microsoft::UI::Xaml::Controls::
        DragItemsStartingEventArgs const &args)
{
  mDragSource = DragSource::Unstaged;
  for (auto item : args.Items()) {
    auto image = item.as<ImageUIData>();
    auto fullPath = winrt::to_string(image.FullPath());
    auto mediumPath = winrt::to_string(image.MediumPath());
    auto smallPath = winrt::to_string(image.SmallPath());
    mDragAndDropSelectedImages.push_back(PB::Thumbnails(
        PB::Path(fullPath), PB::Path(mediumPath), PB::Path(smallPath)));
  }
}

void TableContentPage::OnDragOverStagedPhotos(
    [[maybe_unused]] Windows::Foundation::IInspectable const &sender,
    Microsoft::UI::Xaml::DragEventArgs const                 &args)
{
  args.AcceptedOperation(
      Windows::ApplicationModel::DataTransfer::DataPackageOperation::Copy);
}

void TableContentPage::OnClickedOutsideList(
    [[maybe_unused]] Windows::Foundation::IInspectable const &sender,
    [[maybe_unused]] Microsoft::UI::Xaml::Input::TappedRoutedEventArgs const
        &args)
{
  auto framework = args.OriginalSource().as<FrameworkElement>();

  auto image = framework.DataContext().as<ImageUIData>();

  if (!image) {

    // StagedListView().DeselectRange(Microsoft::UI::Xaml::Data::ItemIndexRange(
    //     0, mStagedImageCollection.Size()));

    // UnstagedListView().DeselectRange(Microsoft::UI::Xaml::Data::ItemIndexRange(
    //     0, mUnstagedImageCollection.Size()));
  }
}
void TableContentPage::UnstagedSelectAllInvoked(
    winrt::Microsoft::UI::Xaml::Input::KeyboardAccelerator const,
    winrt::Microsoft::UI::Xaml::Input::
        KeyboardAcceleratorInvokedEventArgs const)
{
  UnstagedListView().SelectAll();
}

void TableContentPage::UpdateCanvasSize()
{
  // The new size is computed based on the CanvasBorder size

  int width =
      (int)(CanvasBorder().ActualWidth() - CanvasBorder().Padding().Left -
            CanvasBorder().Padding().Right);
  int height =
      (int)(CanvasBorder().ActualHeight() - GalleryBottomName().ActualHeight() -
            CanvasBorder().Padding().Top - CanvasBorder().Padding().Bottom);

  if (width > 0 && height > 0) {
    auto paperSettings = mPhotoBook.paperSettings();

    double ratio = PaperToCanvasRatio(paperSettings.width, paperSettings.height,
                                      width, height);

    auto newWidth = floor((double)paperSettings.width / ratio);
    auto newHeight = floor((double)paperSettings.height / ratio);

    GalleryCanvas().Width(newWidth);
    GalleryCanvas().Height(newHeight);
  }
}

void TableContentPage::OnTableContentSizeChanged(
    [[maybe_unused]] Windows::Foundation::IInspectable const         &sender,
    [[maybe_unused]] Microsoft::UI::Xaml::SizeChangedEventArgs const &args)
{
  UpdateCanvasSize();
  GalleryCanvas().Invalidate();
}

void TableContentPage::OnDropIntoStagedPhotos(
    [[maybe_unused]] Windows::Foundation::IInspectable const  &sender,
    [[maybe_unused]] Microsoft::UI::Xaml::DragEventArgs const &args)
{
  PB::printDebug("Drop on staged list view.\n");

  mPhotoBook.addStagedPhoto(mDragAndDropSelectedImages);

  OnStagedImageAdded(mDragAndDropSelectedImages);

  mDragAndDropSelectedImages.clear();
}

void TableContentPage::Right()
{
  auto &gallery = mPhotoBook.gallery();
  auto  selectedIndex = gallery.selectedIndex();
  if (selectedIndex > -1) {
    int nextIndex = 0;

    if (gallery.photoLine() == PB::PhotoLine::Unstaged) {
      nextIndex = (selectedIndex + 1) % mUnstagedImageCollection.Size();

      UnstagedListView().DeselectRange(
          Microsoft::UI::Xaml::Data::ItemIndexRange(
              0, mUnstagedImageCollection.Size()));
      UnstagedListView().SelectRange({nextIndex, 1});
    }
    else if (gallery.photoLine() == PB::PhotoLine::Staged) {
      nextIndex = (selectedIndex + 1) % mStagedImageCollection.Size();
      StagedListView().DeselectRange(Microsoft::UI::Xaml::Data::ItemIndexRange(
          0, mStagedImageCollection.Size()));
      StagedListView().SelectRange({nextIndex, 1});
    }
  }
}

void TableContentPage::OnGalleryLeft(
    [[maybe_unused]] Windows::Foundation::IInspectable const    &sender,
    [[maybe_unused]] Microsoft::UI::Xaml::RoutedEventArgs const &args)
{
  Left();
  UpdateGalleryLabel();
}

void TableContentPage::Left()
{
  auto &gallery = mPhotoBook.gallery();
  auto  selectedIndex = gallery.selectedIndex();
  if (selectedIndex > -1) {
    int nextIndex = 0;

    if (gallery.photoLine() == PB::PhotoLine::Unstaged) {
      if (selectedIndex == 0) {
        nextIndex = (int)mUnstagedImageCollection.Size() - 1;
      }
      else {
        nextIndex = selectedIndex - 1;
      }

      UnstagedListView().DeselectRange(
          Microsoft::UI::Xaml::Data::ItemIndexRange(
              0, mUnstagedImageCollection.Size()));
      UnstagedListView().SelectRange({nextIndex, 1});
    }
    else if (gallery.photoLine() == PB::PhotoLine::Staged) {
      if (selectedIndex == 0) {
        nextIndex = (int)mStagedImageCollection.Size() - 1;
      }
      else {
        nextIndex = selectedIndex - 1;
      }
      StagedListView().DeselectRange(Microsoft::UI::Xaml::Data::ItemIndexRange(
          0, mStagedImageCollection.Size()));
      StagedListView().SelectRange({nextIndex, 1});
    }
  }
}

void TableContentPage::OnGalleryRight(
    [[maybe_unused]] Windows::Foundation::IInspectable const    &sender,
    [[maybe_unused]] Microsoft::UI::Xaml::RoutedEventArgs const &args)
{
  Right();
  UpdateGalleryLabel();
}

void TableContentPage::OnCanvasDraw(
    [[maybe_unused]] winrt::Microsoft::Graphics::Canvas::UI::Xaml::
        CanvasControl const &sender,
    winrt::Microsoft::Graphics::Canvas::UI::Xaml::CanvasDrawEventArgs const
        &args)
{
  auto session = args.DrawingSession();

  auto &gallery = mPhotoBook.gallery();

  int32_t portviewWidth = (int32_t)GalleryCanvas().ActualWidth();

  int32_t portviewHeight = (int32_t)GalleryCanvas().ActualHeight();

  std::shared_ptr<cv::Mat> image = nullptr;

  auto maybeThumbnail = gallery.selectedItem();

  if (!maybeThumbnail) {
    return;
  }
  auto mediumThumbnailPath = maybeThumbnail->mediumThumbnail;
  if (mediumThumbnailPath.empty()) {
    return;
  }
  if (PB::Process::validExtension(maybeThumbnail->fullPath)) {
    image = PB::Process::singleColorImage(portviewWidth, portviewHeight,
                                          {255, 255, 255})();

    auto tmpImage = mPhotoBook.loadGalleryImage(
        mediumThumbnailPath.string(), {portviewWidth, portviewHeight});
    if (gallery.photoLine() == PB::PhotoLine::Unstaged) {
      PB::Process::overlap(tmpImage, PB::Process::alignToCenter())(image);
    }
    else {
      PB::Process::overlap(tmpImage, PB::Process::alignToCenter())(image);
    }
  }
  else {
    image = PB::Process::singleColorImage(portviewWidth, portviewHeight,
                                          {255, 255, 255})();

    image = PB::Process::addText({portviewWidth / 2, portviewHeight / 2},
                                 maybeThumbnail->fullPath.stem().string(),
                                 {0, 0, 0})(image);
  }

  auto device = CanvasDevice::GetSharedDevice();

  winrt::array_view<uint8_t const> view_name(
      (uint8_t *)image->data, (uint32_t)(image->total() * image->elemSize()));

  const auto format =
      Microsoft::Graphics::DirectX::DirectXPixelFormat::B8G8R8A8UIntNormalized;

  auto bitmap =
      winrt::Microsoft::Graphics::Canvas::CanvasBitmap::CreateFromBytes(
          device, view_name, (int32_t)image->cols, (int32_t)image->rows,
          (winrt::Windows::Graphics::DirectX::DirectXPixelFormat const &)
              format);

  session.DrawImage(bitmap);
}

void TableContentPage::OnMappingFinished(PB::Path rootPath)
{
  mNavigationItemsCollection.Append(
      winrt::to_hstring(rootPath.filename().string()));

  MediaListView().ItemsSource(mNavigationItemsCollection);

  auto rootFolders = mPhotoBook.imageSupport().groups();

  MediaListView().SelectedIndex(mNavigationItemsCollection.Size() - 1);
}

void TableContentPage::OnThumbnailsProcessingFinished(PB::Path rootPath)
{
  mLoadedFinishedImportFolders.insert(rootPath);
  auto selectedIndex = mPhotoBook.gallery().selectedIndex();
  auto maybeGroupPath = mPhotoBook.imageSupport().groupByIndex(selectedIndex);

  if (maybeGroupPath && maybeGroupPath.value() == rootPath) {
    UpdateUnstagedImagesView(selectedIndex);
  }
  StatusLabelText().Text(winrt::to_hstring("Status: Idle"));
  MainProgressBar().Visibility(
      winrt::Microsoft::UI::Xaml::Visibility::Collapsed);

  PB::printDebug("Index selected %d\n",
                 (int)(mNavigationItemsCollection.Size() - 1));
}

void TableContentPage::OnImportSelectionChanged(
    [[maybe_unused]] ::winrt::Windows::Foundation::IInspectable const &,
    [[maybe_unused]] ::winrt::Microsoft::UI::Xaml::Controls::
        SelectionChangedEventArgs const &)
{
  auto index = MediaListView().SelectedIndex();

  PB::printDebug("Index selected %d\n", index);

  auto &imagesData = mPhotoBook.imageSupport();
  auto  iterator = imagesData.unstagedIterator(index);

  if (iterator.valid()) {
    int diff = (int)mUnstagedImageCollection.Size() - (int)iterator.size();
    if (diff > 0) {
      for (int i = 0; i < diff; ++i) {
        mUnstagedImageCollection.RemoveAtEnd();
      }
    }
    else if (diff < 0) {
      for (int i = 0; i < -diff; ++i) {
        mUnstagedImageCollection.Append(ImageUIData());
      }
    }
    UpdateUnstagedImagesView(index);

    mPhotoBook.gallery().selectImportFolder(index, iterator);

    UpdateGalleryLabel();
  }
}

void TableContentPage::UpdateUnstagedImagesView(int index)
{
  auto &imagesData = mPhotoBook.imageSupport();
  auto  iterator = imagesData.unstagedIterator(index);

  auto rootPath = mPhotoBook.imageSupport().groupByIndex(index);
  if (rootPath && mLoadedFinishedImportFolders.find(rootPath.value()) !=
                      mLoadedFinishedImportFolders.end()) {
    auto size = iterator.size();
    for (int i = 0; i < (int)size; ++i) {
      PB::Thumbnails thumbnail = iterator[i].current().value();
      mUnstagedImageCollection.SetAt(
          i, ImageUIData(winrt::to_hstring(thumbnail.fullPath.string()),
                         winrt::to_hstring(thumbnail.mediumThumbnail.string()),
                         winrt::to_hstring(thumbnail.smallThumbnail.string())));
    }
  }
}

void TableContentPage::OnUnstagedPhotosSelectionChanged(
    [[maybe_unused]] ::winrt::Windows::Foundation::IInspectable const &,
    [[maybe_unused]] ::winrt::Microsoft::UI::Xaml::Controls::
        SelectionChangedEventArgs const &)
{
  StagedListView().DeselectRange(Microsoft::UI::Xaml::Data::ItemIndexRange(
      0, mStagedImageCollection.Size()));

  auto navigationListIndex = MediaListView().SelectedIndex();

  if (navigationListIndex < 0) {
    return;
  }

  auto unstagedPhotoIndex = UnstagedListView().SelectedIndex();

  auto &imagesData = mPhotoBook.imageSupport();

  auto iterator =
      imagesData.unstagedIterator(navigationListIndex, unstagedPhotoIndex);
  mPhotoBook.gallery().selectImportFolder(unstagedPhotoIndex, iterator);

  if (unstagedPhotoIndex > -1) {
    mPhotoBook.gallery().setPosition(unstagedPhotoIndex);
    UpdateGalleryLabel();
  }
}

void TableContentPage::OnStagedPhotosSelectionChanged(
    [[maybe_unused]] ::winrt::Windows::Foundation::IInspectable const &,
    [[maybe_unused]] ::winrt::Microsoft::UI::Xaml::Controls::
        SelectionChangedEventArgs const &)
{
  UnstagedListView().DeselectRange(Microsoft::UI::Xaml::Data::ItemIndexRange(
      0, mUnstagedImageCollection.Size()));

  auto stagedImagesIndex = StagedListView().SelectedIndex();
  if (stagedImagesIndex < 0) {
    return;
  }
  auto &imagesData = mPhotoBook.imageSupport();

  auto iterator = imagesData.stagedIterator();
  iterator = iterator[stagedImagesIndex];

  mPhotoBook.gallery().selectStagedPhotos(stagedImagesIndex, iterator);
  UpdateGalleryLabel();
}

void TableContentPage::OnStagedDragItemsCompleted(
    [[maybe_unused]] ::winrt::Windows::Foundation::IInspectable const &obj,
    [[maybe_unused]] ::winrt::Microsoft::UI::Xaml::Controls::
        DragItemsCompletedEventArgs const &args)
{
  if (mDragSource == DragSource::Staged) {
    mDragSource = DragSource::None;
  }
}

void TableContentPage::OnStagedDragItemsStarting(
    [[maybe_unused]] ::winrt::Windows::Foundation::IInspectable const &,
    [[maybe_unused]] ::winrt::Microsoft::UI::Xaml::Controls::
        DragItemsStartingEventArgs const &args)
{
  mDragSource = DragSource::Staged;
}

void TableContentPage::OnMappingStopped() {}

void TableContentPage::OnMappingStarted() {}

void TableContentPage::OnMappingPaused() {}

void TableContentPage::OnMappingResumed() {}

void TableContentPage::OnProgressUpdate(PB::Path rootPath, int progress,
                                        int reference)
{
  auto selectedIndex = mPhotoBook.gallery().selectedIndex();
  auto selectedRootPath = mPhotoBook.imageSupport().groupByIndex(selectedIndex);
  if (selectedRootPath && rootPath == selectedRootPath.value()) {
    MainProgressBar().Visibility(
        winrt::Microsoft::UI::Xaml::Visibility::Visible);
    MainProgressBar().Maximum(reference);
    MainProgressBar().Value(progress);
    StatusLabelText().Text(winrt::to_hstring("Status: In progress..."));
  }
}

void TableContentPage::OnExportProgressUpdate(int progress, int reference)
{
  MainProgressBar().Visibility(winrt::Microsoft::UI::Xaml::Visibility::Visible);
  MainProgressBar().Maximum(reference);
  MainProgressBar().Value(progress);
  StatusLabelText().Text(winrt::to_hstring("Status: In progress..."));
}

void TableContentPage::OnExportFinished()
{
  StatusLabelText().Text(winrt::to_hstring("Status: Idle"));
  MainProgressBar().Visibility(
      winrt::Microsoft::UI::Xaml::Visibility::Collapsed);
}

void TableContentPage::OnUnstagedImageAdded(PB::Path rootPath,
                                            PB::Path fullPath,
                                            PB::Path mediumPath,
                                            PB::Path smallPath, int position)
{
  auto selectedIndex = mPhotoBook.gallery().selectedIndex();
  auto selectedRootPath = mPhotoBook.imageSupport().groupByIndex(selectedIndex);
  if (selectedRootPath && rootPath == selectedRootPath) {
    mUnstagedImageCollection.SetAt(
        position, ImageUIData(winrt::to_hstring(fullPath.string()),
                              winrt::to_hstring(mediumPath.string()),
                              winrt::to_hstring(smallPath.string())));
  }

  if (mStagedImages.contains(fullPath)) {
    for (int i = 0; i < (int)mStagedImageCollection.Size(); ++i) {
      auto path = winrt::to_string(mStagedImageCollection.GetAt(i).FullPath());
      if (PB::Path(path) == fullPath) {
        mStagedImageCollection.SetAt(
            i, ImageUIData(winrt::to_hstring(fullPath.string()),
                           winrt::to_hstring(mediumPath.string()),
                           winrt::to_hstring(smallPath.string())));
      }
    }
  }
}

void TableContentPage::OnAddingUnstagedImagePlaceholder(unsigned size)
{
  for (int i = 0; i < (int)size; ++i) {
    mUnstagedImageCollection.Append(ImageUIData());
  }
}

void TableContentPage::OnStagedImageAdded(std::vector<PB::Thumbnails> photos,
                                          int                         index)
{
  PB::basicAssert(index == -1);
  if (index == -1) {
    for (auto &photo : photos) {
      ImageUIData winRTImage(winrt::to_hstring(photo.fullPath.string()),
                             winrt::to_hstring(photo.mediumThumbnail.string()),
                             winrt::to_hstring(photo.smallThumbnail.string()));
      mStagedImageCollection.Append(winRTImage);
      mStagedImages.insert(photo.fullPath);
    }
  }
}

void TableContentPage::OnStagedImageRemoved(std::vector<int> removedIndexes)
{
  std::sort(removedIndexes.begin(), removedIndexes.end(), std::greater<int>());
  for (int i = 0; i < removedIndexes.size(); ++i) {
    auto path = winrt::to_string(
        mStagedImageCollection.GetAt(removedIndexes.at(i)).FullPath());
    mStagedImageCollection.RemoveAt(removedIndexes.at(i));
    mStagedImages.erase(PB::Path(path));
  }
}

void TableContentPage::OnError(PB::Error error)
{
  GenericErrorTextBlock().Text(winrt::to_hstring(error.description()));
  Post([this]() { GenericErrorDialogDisplay(); });
}

void TableContentPage::Post(std::function<void()> f)
{
  bool success = MainWindow::sMainthreadDispatcher.TryEnqueue(
      DispatcherQueuePriority::Normal, [f{f}]() { f(); });
  PB::basicAssert(success);
}

void TableContentPage::UpdateGalleryLabel()
{
  auto &gallery = mPhotoBook.gallery();
  auto  itemPath = gallery.selectedItem();
  auto  importFolderIndex = gallery.selectedIndex();
  auto  rootName = mPhotoBook.imageSupport().groupByIndex(importFolderIndex);

  GalleryLeftButton().IsEnabled(itemPath.has_value());
  GalleryRightButton().IsEnabled(itemPath.has_value());

  if (itemPath) {
    GalleryMainText().Text(
        winrt::to_hstring(itemPath->fullPath.filename().string()));
  }
  else if (rootName) {
    GalleryMainText().Text(winrt::to_hstring(rootName->filename().string()));
  }
  else {
    GalleryMainText().Text(winrt::to_hstring("Nothing sleected."));
  }
  GalleryCanvas().Invalidate();
}

void TableContentPage::PostponeError(std::string message)
{
  GenericErrorTextBlock().Text(winrt::to_hstring(message));
  Post([this]() { GenericErrorDialogDisplay(); });
}

void TableContentPage::OnNavigatedTo(
    Microsoft::UI::Xaml::Navigation::NavigationEventArgs e)
{
  std::string fullPath =
      winrt::to_string(winrt::unbox_value<winrt::hstring>(e.Parameter()));

  PB::FilePersistence projectPersistence(fullPath);
  projectPersistence.read(
      [this](std::variant<PB::Json, PB::Error> jsonOrError) {
        if (std::holds_alternative<PB::Error>(jsonOrError)) {
          auto error = std::get<PB::Error>(jsonOrError);
          Frame().Navigate(winrt::xaml_typename<PhotobookUI::Dashboard>());
        }
        else {
          auto &json = std::get<PB::Json>(jsonOrError);
          auto  projectDetailsOrError =
              PB::Text::deserialize<PB::ProjectDetails>(json);

          if (std::holds_alternative<PB::Error>(projectDetailsOrError)) {
            Frame().Navigate(winrt::xaml_typename<PhotobookUI::Dashboard>());
          }
          else {
            auto &projectDetails =
                std::get<PB::ProjectDetails>(projectDetailsOrError);
            OnProjectDetailsLoaded(projectDetails);
          }
        }
      });
}

void TableContentPage::OnProjectDetailsLoaded(PB::ProjectDetails projectDetails)
{
  mPhotoBook.configureProject(PB::Project(projectDetails));
}

void TableContentPage::OnExportClicked(
    [[maybe_unused]] Windows::Foundation::IInspectable const &,
    [[maybe_unused]] Microsoft::UI::Xaml::RoutedEventArgs const &)
{
  ExportDialogDisplay();
}

void TableContentPage::OnContentDialogSaveClicked(
    [[maybe_unused]] Windows::Foundation::IInspectable const &,
    [[maybe_unused]] Microsoft::UI::Xaml::Controls::
        ContentDialogButtonClickEventArgs const &)
{
  mPopups.fireSaveFilePicker(
      MainWindow::sMainWindowhandle,
      [this](std::variant<std::string, PB::Error> result) {
        if (std::holds_alternative<std::string>(result)) {
          auto &newName = std::get<std::string>(result);

          mPhotoBook.savePhotobook(newName);
          mPhotoBook.discardPhotobook();
          Frame().Navigate(winrt::xaml_typename<PhotobookUI::Dashboard>());
        }
        else {
          OnError(std::get<PB::Error>(result));
        }
        if (mExitFlag) {
          Post([]() {
            winrt::Microsoft::UI::Xaml::Application::Current().Exit();
          });
        }
      });
}

void TableContentPage::OnExportContentDialogClicked(
    [[maybe_unused]] Windows::Foundation::IInspectable const &sender,
    [[maybe_unused]] Microsoft::UI::Xaml::Controls::
        ContentDialogButtonClickEventArgs const &args)
{
  auto        exportName = ExportNameTextBox().Text();
  std::string nativeExportName = winrt::to_string(exportName);

  if (!OptionPDFCheckBox().IsChecked()) {
    PostponeError("Check a format!");
  }

  if (nativeExportName.empty()) {
    PostponeError("The given name must not be empty!");
  }
  else if (mStagedImageCollection.Size() == 0) {
    PostponeError("There is no staged photo!");
  }
  else {
    mPopups.fireFolderPicker(
        MainWindow::sMainWindowhandle, [this, nativeExportName](PB::Path path) {
          Post([this, path{path}, nativeExportName{nativeExportName}]() {
            mPhotoBook.exportAlbum(nativeExportName, path);
          });
        });
  }
}

void TableContentPage::OnContentDialogDiscardClicked(
    [[maybe_unused]] Windows::Foundation::IInspectable const &,
    [[maybe_unused]] Microsoft::UI::Xaml::Controls::
        ContentDialogButtonClickEventArgs const &)
{
  mPhotoBook.discardPhotobook();
  if (mExitFlag) {
    Post([]() { winrt::Microsoft::UI::Xaml::Application::Current().Exit(); });
  }
  else {
    Frame().Navigate(winrt::xaml_typename<PhotobookUI::Dashboard>());
  }
}

void TableContentPage::OnContentDialogCancelClicked(
    [[maybe_unused]] Windows::Foundation::IInspectable const &,
    [[maybe_unused]] Microsoft::UI::Xaml::Controls::
        ContentDialogButtonClickEventArgs const &)
{
}

} // namespace winrt::PhotobookUI::implementation
