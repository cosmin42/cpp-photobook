// Copyright (c) Microsoft Corporation and Contributors.
// Licensed under the MIT License.

// clang-format off
#include "pch.h"
#include "TableContentPage.xaml.h"
#if __has_include("TableContentPage.g.cpp")
#include "TableContentPage.g.cpp"
#endif
// clang-format on
#include "App.xaml.h"

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
#include <pb/image/RegularImage.h>
#include <pb/SerializationStrategy.h>
#include <pb/config/Log.h>

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

Path TableContentPage::CurrentAppLocation()
{
  StorageFolder folder = ApplicationData::Current().LocalFolder();

  return Path(winrt::to_string(folder.Path()));
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
    : mListener(std::make_shared<PhotobookTableListener>(std::ref(*this))),
      mPhotoBook(Application::Current()
                     .as<winrt::PhotobookUI::implementation::App>()
                     ->api())
{
  InitializeComponent();

  mPhotoBook->configure(ScreenSize());
  mPhotoBook->configure(mListener);
  mPhotoBook->configure((PB::StagedImagesListener *)this);
  mPhotoBook->configure((PB::ImageMonitorListener *)this);

  mNavigationItemsCollection =
      winrt::single_threaded_observable_vector<winrt::hstring>();
  mUnstagedImageCollection =
      winrt::single_threaded_observable_vector<ImageUIData>();
  mStagedImageCollection =
      winrt::single_threaded_observable_vector<ImageUIData>();

  MainWindow::sMainExitFunction = [this]() {
    auto projectDetails = mPhotoBook->projectDetails();
    bool alreadySaved = mPhotoBook->persistence()->isSaved(projectDetails);
    if (!alreadySaved) {
      ProjectExitDialogDisplay();
      mExitFlag = true;
    }
    else {
      Post([]() { winrt::Microsoft::UI::Xaml::Application::Current().Exit(); });
    }
  };

  UnstagedListView().ItemsSource(mUnstagedImageCollection);
  StagedListView().ItemsSource(mStagedImageCollection);

  KeyUp([this](Windows::Foundation::IInspectable const              &sender,
               Microsoft::UI::Xaml::Input::KeyRoutedEventArgs const &arg) {
    OnKeyPressed(sender, arg);
  });

  mStagedImageCollection.VectorChanged(
      [this](IObservableVector<ImageUIData> const &sender,
             IVectorChangedEventArgs const        &args) {
        PB::printDebug(
            "Reorder changed index: %d %s\n", args.Index(),
            std::string(magic_enum::enum_name(args.CollectionChange()))
                .c_str());
        auto changeType = args.CollectionChange();
        int  index = (int)args.Index();
        OnStagedImageCollectionChanged(sender, changeType, index);
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

TableContentPage::~TableContentPage()
{
  MainWindow::sMainExitFunction = nullptr;
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
  PBDev::basicAssert(boundingBoxWidth > 0);
  PBDev::basicAssert(boundingBoxHeight > 0);
  double widthRatio = (double)width / (double)boundingBoxWidth;
  double heightRatio = (double)height / (double)boundingBoxHeight;

  double maxRatio = std::max<double>(widthRatio, heightRatio);

  return maxRatio;
}

int TableContentPage::CanvasMinWidth()
{
  auto paperSettings = mPhotoBook->activeProject().paperSettings();
  PBDev::basicAssert(mPhotoBook->activeProject().paperSettings().ppi > 0);

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
  auto paperSettings = mPhotoBook->activeProject().paperSettings();
  PBDev::basicAssert(mPhotoBook->activeProject().paperSettings().ppi > 0);

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
      MainWindow::sMainWindowHandle,
      [this](Path path) { mPhotoBook->addImportFolder(path); });
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
    Path fullPath =
        winrt::to_string(mStagedImageCollection.GetAt(i).FullPath());

    if (mPhotoBook->imageViews().imageMonitor().rowIndex(fullPath) ==
        (unsigned)selectedIndex) {
      // TODO: Optimize this
      mPhotoBook->imageViews().stagedImages().removePicture({(unsigned)i});
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

  mPhotoBook->imageViews().imageMonitor().removeRow((int)selectedIndex);

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
  auto projectDetails = mPhotoBook->projectDetails();
  bool alreadySaved = mPhotoBook->persistence()->isSaved(projectDetails);
  if (alreadySaved) {
    mPhotoBook->discardPhotobook();
    Frame().Navigate(winrt::xaml_typename<PhotobookUI::Dashboard>());
  }
  else {
    ProjectExitDialogDisplay();
  }
}

void TableContentPage::OnAboutClicked(
    [[maybe_unused]] Windows::Foundation::IInspectable const    &sender,
    [[maybe_unused]] Microsoft::UI::Xaml::RoutedEventArgs const &args)
{
  GenericMessageTextBlock().Text(
      winrt::to_hstring("Version: " + std::string(PB::Context::VERSION) +
                        "\nEmail: cosmin@mandelbrot-code.com"));
  GenericMessageDialogDisplay();
}

void TableContentPage::OnLicenseClicked(
    [[maybe_unused]] Windows::Foundation::IInspectable const    &sender,
    [[maybe_unused]] Microsoft::UI::Xaml::RoutedEventArgs const &args)
{
  auto installFolder =
      Windows::ApplicationModel::Package::Current().InstalledLocation();

  Path installedLocation = winrt::to_string(installFolder.Path());

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
  Post([]() { winrt::Microsoft::UI::Xaml::Application::Current().Exit(); });
  mExitFlag = true;
}

void TableContentPage::OnSaveClicked(
    [[maybe_unused]] Windows::Foundation::IInspectable const    &sender,
    [[maybe_unused]] Microsoft::UI::Xaml::RoutedEventArgs const &args)
{
  auto projectDetails = mPhotoBook->projectDetails();
  bool alreadySaved = mPhotoBook->persistence()->isSaved(projectDetails);
  if (alreadySaved) {
    return;
  }
  if (!mPhotoBook->projectDefaultSaved()) {
    mPhotoBook->savePhotobook();
  }
  else {
    mPopups.fireSaveFilePicker(
        MainWindow::sMainWindowHandle,
        [this](std::variant<std::string, PBDev::Error> result) {
          if (std::holds_alternative<std::string>(result)) {
            auto &newName = std::get<std::string>(result);
            mPhotoBook->savePhotobook(newName);
          }
          else {
            OnError(std::get<PBDev::Error>(result));
          }
        });
  }
}

void TableContentPage::OnSaveAsClicked(
    [[maybe_unused]] Windows::Foundation::IInspectable const    &sender,
    [[maybe_unused]] Microsoft::UI::Xaml::RoutedEventArgs const &args)
{
  mPopups.fireSaveFilePicker(
      MainWindow::sMainWindowHandle,
      [this](std::variant<std::string, PBDev::Error> result) {
        if (std::holds_alternative<std::string>(result)) {
          auto &newName = std::get<std::string>(result);
          mPhotoBook->savePhotobook(newName);
        }
        else {
          OnError(std::get<PBDev::Error>(result));
        }
      });
}

void TableContentPage::OnNewClicked(
    [[maybe_unused]] Windows::Foundation::IInspectable const    &sender,
    [[maybe_unused]] Microsoft::UI::Xaml::RoutedEventArgs const &args)
{
  auto projectDetails = mPhotoBook->projectDetails();
  bool alreadySaved = mPhotoBook->persistence()->isSaved(projectDetails);
  if (alreadySaved) {
    mPhotoBook->discardPhotobook();
    Frame().Navigate(winrt::xaml_typename<PhotobookUI::Dashboard>(),
                     winrt::box_value(winrt::to_hstring("new-project")));
  }
  else {
    mPopups.fireSaveFilePicker(
        MainWindow::sMainWindowHandle,
        [this](std::variant<std::string, PBDev::Error> result) {
          if (std::holds_alternative<std::string>(result)) {
            auto &newName = std::get<std::string>(result);

            mPhotoBook->savePhotobook(newName);
            mPhotoBook->discardPhotobook();
            Frame().Navigate(
                winrt::xaml_typename<PhotobookUI::Dashboard>(),
                winrt::box_value(winrt::to_hstring("new-project")));
          }
          else {
            OnError(std::get<PBDev::Error>(result));
          }
        });
  }
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
  UISelectionIndex selectionIndex = SelectionIndex();
  if (selectionIndex.stagedPhotoIndex.empty() &&
      selectionIndex.unstagedLineIndex.empty()) {
    return;
  }

  Windows::System::VirtualKey key = arg.Key();

  switch (key) {
  case Windows::System::VirtualKey::Delete: {
    auto                  ranges = StagedListView().SelectedRanges();
    std::vector<unsigned> selectedIndexes;
    for (auto range : ranges) {
      for (auto i = range.FirstIndex(); i <= range.LastIndex(); ++i) {
        selectedIndexes.push_back(i);
      }
    }

    if (selectedIndexes.size() > 0) {
      mPhotoBook->imageViews().stagedImages().removePicture(selectedIndexes);
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

    auto regularImage = std::make_shared<PB::RegularImage>(PB::Thumbnails(
        Path(fullPath), Path(mediumPath), Path(smallPath)));

    mDragAndDropSelectedImages.push_back(regularImage);
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
    auto paperSettings = mPhotoBook->activeProject().paperSettings();

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

  // WORKAROUND
  // https://stackoverflow.com/questions/77401865/winui3-listview-insertionmark-functionality-with-c-winrt/77406535#77406535
  auto dropRelativeToStageListViewPosition =
      args.GetPosition(sender.as<Microsoft::UI::Xaml::Controls::ListView>());
  PB::printDebug("(%f %f)\n", dropRelativeToStageListViewPosition.X,
                 dropRelativeToStageListViewPosition.Y);

  if (dropRelativeToStageListViewPosition.Y < 0 ||
      dropRelativeToStageListViewPosition.Y > StagedListView().ActualHeight()) {
    return;
  }

  int insertPostion = StagedListView().Items().Size();

  for (auto item : StagedListView().Items()) {
    auto itemContainer = StagedListView()
                             .ContainerFromItem(item)
                             .as<Microsoft::UI::Xaml::Controls::ListViewItem>();
    auto itemIndex = StagedListView().IndexFromContainer(itemContainer);

    auto dropRelativeToItem = args.GetPosition(itemContainer);

    PB::printDebug("(%f %f)\n", dropRelativeToItem.X, dropRelativeToItem.Y);

    if (dropRelativeToItem.X < itemContainer.ActualWidth()) {
      insertPostion = itemIndex;
      break;
    }
  }

  PB::printDebug("Insertion position: %d\n", insertPostion);

  // WORKAROUND
  mPhotoBook->imageViews().stagedImages().addPictures(
      mDragAndDropSelectedImages, insertPostion);
  // mPhotoBook->imageSupport().stagePhoto(mDragAndDropSelectedImages,
  //                                       insertPostion);

  OnStagedImageAdded(mDragAndDropSelectedImages, insertPostion);

  mDragAndDropSelectedImages.clear();
}

void TableContentPage::Right()
{
  auto selection = SelectionIndex();

  int nextIndex = 0;

  if (!selection.unstagedLineIndex.empty()) {
    PBDev::basicAssert(selection.unstagedLineIndex.size() == 1);

    nextIndex = (SelectionIndex().importListIndex.value() + 1) %
                mUnstagedImageCollection.Size();

    UnstagedListView().DeselectRange(Microsoft::UI::Xaml::Data::ItemIndexRange(
        0, mUnstagedImageCollection.Size()));
    UnstagedListView().SelectRange({nextIndex, 1});
  }
  else if (!selection.stagedPhotoIndex.empty()) {
    PBDev::basicAssert(selection.stagedPhotoIndex.size() == 1);
    nextIndex = (SelectionIndex().importListIndex.value() + 1) %
                mStagedImageCollection.Size();
    StagedListView().DeselectRange(Microsoft::UI::Xaml::Data::ItemIndexRange(
        0, mStagedImageCollection.Size()));
    StagedListView().SelectRange({nextIndex, 1});
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
  auto selection = SelectionIndex();
  int  nextIndex = 0;

  if (!selection.unstagedLineIndex.empty()) {
    PBDev::basicAssert(selection.unstagedLineIndex.size() == 1);
    if (selection.unstagedLineIndex.at(0) == 0) {
      nextIndex = (int)mUnstagedImageCollection.Size() - 1;
    }
    else {
      nextIndex = selection.unstagedLineIndex.at(0) - 1;
    }

    UnstagedListView().DeselectRange(Microsoft::UI::Xaml::Data::ItemIndexRange(
        0, mUnstagedImageCollection.Size()));
    UnstagedListView().SelectRange({nextIndex, 1});
  }
  else if (!selection.stagedPhotoIndex.empty()) {
    PBDev::basicAssert(selection.stagedPhotoIndex.size() == 1);
    if (selection.stagedPhotoIndex.at(0) == 0) {
      nextIndex = (int)mStagedImageCollection.Size() - 1;
    }
    else {
      nextIndex = selection.stagedPhotoIndex.at(0) - 1;
    }
    StagedListView().DeselectRange(Microsoft::UI::Xaml::Data::ItemIndexRange(
        0, mStagedImageCollection.Size()));
    StagedListView().SelectRange({nextIndex, 1});
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
  auto selection = SelectionIndex();

  auto session = args.DrawingSession();

  int32_t portviewWidth = (int32_t)GalleryCanvas().ActualWidth();

  int32_t portviewHeight = (int32_t)GalleryCanvas().ActualHeight();

  std::shared_ptr<cv::Mat> image = nullptr;

  if (selection.stagedPhotoIndex.empty() &&
      selection.unstagedLineIndex.empty()) {
    return;
  }

  std::shared_ptr<PB::VirtualImage> imagePtr = nullptr;

  if (!selection.stagedPhotoIndex.empty()) {
    imagePtr = mPhotoBook->imageViews().stagedImages().picture(
        selection.stagedPhotoIndex.at(0));
  }

  auto mediumThumbnailPath = imagePtr->resources().medium;
  if (mediumThumbnailPath.empty()) {
    return;
  }
  if (PB::Process::validExtension(imagePtr->resources().full)) {
    image = PB::Process::singleColorImage(portviewWidth, portviewHeight,
                                          {255, 255, 255})();

    auto tmpImage = PB::ImageReader().read(mediumThumbnailPath.string(),
                                           {portviewWidth, portviewHeight});

    PB::Process::overlap(tmpImage, PB::Process::alignToCenter())(image);
  }
  else {
    image = PB::Process::singleColorImage(portviewWidth, portviewHeight,
                                          {255, 255, 255})();

    image = PB::Process::addText({portviewWidth / 2, portviewHeight / 2},
                                 imagePtr->resources().full.stem().string(),
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

void TableContentPage::OnMappingFinished(Path rootPath)
{
  mNavigationItemsCollection.Append(
      winrt::to_hstring(rootPath.filename().string()));

  MediaListView().ItemsSource(mNavigationItemsCollection);

  MediaListView().SelectedIndex(mNavigationItemsCollection.Size() - 1);
}

void TableContentPage::OnThumbnailsProcessingFinished(Path rootPath)
{
  mLoadedFinishedImportFolders.insert(rootPath);

  auto selection = SelectionIndex();

  auto selectedIndex = selection.importListIndex;
  auto rowPath =
      mPhotoBook->imageViews().imageMonitor().rowPath(selectedIndex.value());

  if (rowPath == rootPath) {
    UpdateUnstagedImagesView(selectedIndex.value());
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
  auto iterator =
      mPhotoBook->imageViews().imageMonitor().statefulIterator(index);

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

    UpdateGalleryLabel();
  }
}

void TableContentPage::OnStagedImageCollectionChanged(
    IObservableVector<ImageUIData> const                     &sender,
    winrt::Windows::Foundation::Collections::CollectionChange changeType,
    int                                                       changedIndex)
{
  if (mDragSource != DragSource::Staged) {
    return;
  }
  if (changeType ==
      winrt::Windows::Foundation::Collections::CollectionChange::ItemInserted) {
    auto image = sender.GetAt(changedIndex);
    auto fullPath = winrt::to_string(image.FullPath());
    auto mediumPath = winrt::to_string(image.MediumPath());
    auto smallPath = winrt::to_string(image.SmallPath());
    auto regularImage = std::make_shared<PB::RegularImage>(PB::Thumbnails(
        Path(fullPath), Path(mediumPath), Path(smallPath)));
    // mPhotoBook->imageSupport().stagePhoto({regularImage}, changedIndex);
    mPhotoBook->imageViews().stagedImages().addPictures({regularImage},
                                                        changedIndex);
  }
  else if (changeType == winrt::Windows::Foundation::Collections::
                             CollectionChange::ItemRemoved) {

    // mPhotoBook->imageSupport().unstagePhoto({changedIndex});
    mPhotoBook->imageViews().stagedImages().removePicture(
        {(unsigned)changedIndex});
  }
  else if (changeType == winrt::Windows::Foundation::Collections::
                             CollectionChange::ItemChanged) {
    PBDev::basicAssert(false);
  }
  else {
    PBDev::basicAssert(false);
  }
}

void TableContentPage::UpdateUnstagedImagesView(int index)
{
  auto iterator =
      mPhotoBook->imageViews().imageMonitor().statefulIterator((unsigned)index);

  if (!iterator.valid()) {
    return;
  }

  auto rootPath = mPhotoBook->imageViews().imageMonitor().rowPath(index);
  if (mLoadedFinishedImportFolders.find(rootPath) !=
      mLoadedFinishedImportFolders.end()) {
    auto size = iterator.size();
    for (int i = 0; i < (int)size; ++i) {
      auto virtualImage = iterator[i].current();
      mUnstagedImageCollection.SetAt(
          i, ImageUIData(
                 winrt::to_hstring(virtualImage->resources().full.string()),
                 winrt::to_hstring(virtualImage->resources().medium.string()),
                 winrt::to_hstring(virtualImage->resources().small.string())));
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

  if (unstagedPhotoIndex > -1) {
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

void TableContentPage::OnProgressUpdate(Path rootPath, int progress,
                                        int reference)
{
  auto selection = SelectionIndex();

  auto selectedRootPath = mPhotoBook->imageViews().imageMonitor().rowPath(
      selection.unstagedLineIndex.at(0));

  if (rootPath == selectedRootPath) {
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

void TableContentPage::OnUnstagedImageAdded(Path rootPath,
                                            Path fullPath,
                                            Path mediumPath,
                                            Path smallPath, int position)
{
  auto selection = SelectionIndex();

  auto selectedRootPath = mPhotoBook->imageViews().imageMonitor().rowPath(
      selection.unstagedLineIndex.at(0));

  if (rootPath == selectedRootPath) {
    mUnstagedImageCollection.SetAt(
        position, ImageUIData(winrt::to_hstring(fullPath.string()),
                              winrt::to_hstring(mediumPath.string()),
                              winrt::to_hstring(smallPath.string())));
  }

  if (mStagedImages.contains(fullPath)) {
    for (int i = 0; i < (int)mStagedImageCollection.Size(); ++i) {
      auto path = winrt::to_string(mStagedImageCollection.GetAt(i).FullPath());
      if (Path(path) == fullPath) {
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

void TableContentPage::OnStagedImageAdded(
    std::vector<std::shared_ptr<PB::VirtualImage>> photos, int index)
{
  PBDev::basicAssert(index <= (int)mStagedImageCollection.Size());

  if (index == (int)mStagedImageCollection.Size() || index < 0) {
    for (auto photo : photos) {
      ImageUIData winRTImage(
          winrt::to_hstring(photo->resources().full.string()),
          winrt::to_hstring(photo->resources().medium.string()),
          winrt::to_hstring(photo->resources().small.string()));
      mStagedImageCollection.Append(winRTImage);
      mStagedImages.insert(photo->resources().full.string());
    }
  }
  else if (index < (int)mStagedImageCollection.Size()) {
    for (auto photo : photos) {
      ImageUIData winRTImage(
          winrt::to_hstring(photo->resources().full.string()),
          winrt::to_hstring(photo->resources().medium.string()),
          winrt::to_hstring(photo->resources().small.string()));
      mStagedImageCollection.InsertAt(index, winRTImage);
      mStagedImages.insert(photo->resources().full.string());
    }
  }
}

void TableContentPage::OnStagedImageRemoved(
    std::vector<unsigned> removedIndexes)
{
  std::sort(removedIndexes.begin(), removedIndexes.end(), std::greater<int>());
  for (int i = 0; i < removedIndexes.size(); ++i) {
    auto path = winrt::to_string(
        mStagedImageCollection.GetAt(removedIndexes.at(i)).FullPath());
    mStagedImageCollection.RemoveAt(removedIndexes.at(i));
    mStagedImages.erase(Path(path));
  }
}

void TableContentPage::OnError(PBDev::Error error)
{
  GenericErrorTextBlock().Text(winrt::to_hstring(error.description()));
  Post([this]() { GenericErrorDialogDisplay(); });
}

void TableContentPage::Post(std::function<void()> f)
{
  bool success = MainWindow::sMainThreadDispatcher.TryEnqueue(
      DispatcherQueuePriority::Normal, [f{f}]() { f(); });
  PBDev::basicAssert(success);
}

void TableContentPage::onPicturesAdded(int index, int size) {}

void TableContentPage::onPictureRemoved(std::vector<unsigned> index) {}

void TableContentPage::onImportFolderAdded() {}

void TableContentPage::onImportFolderRemoved(unsigned index) {}

void TableContentPage::onCleared() {}

UISelectionIndex TableContentPage::SelectionIndex()
{
  UISelectionIndex selectionIndex;

  auto ranges = UnstagedListView().SelectedRanges();

  for (auto range : ranges) {
    for (auto i = range.FirstIndex(); i <= range.LastIndex(); ++i) {
      selectionIndex.unstagedLineIndex.push_back(i);
    }
  }

  ranges = StagedListView().SelectedRanges();
  for (auto range : ranges) {
    for (auto i = range.FirstIndex(); i <= range.LastIndex(); ++i) {
      selectionIndex.stagedPhotoIndex.push_back(i);
    }
  }

  auto index = MediaListView().SelectedIndex();

  if (index >= 0) {
    selectionIndex.importListIndex = (unsigned)index;
  }
  return selectionIndex;
}

bool TableContentPage::StagedLineEmpty()
{
  return StagedListView().Items().Size() == 0;
}

bool TableContentPage::UnstagedLintEmpty()
{
  return UnstagedListView().Items().Size() == 0;
}

void TableContentPage::UpdateGalleryLabel()
{
  auto selection = SelectionIndex();

  auto selectedRootPath = mPhotoBook->imageViews().imageMonitor().rowPath(
      selection.importListIndex.value());

  std::shared_ptr<PB::VirtualImage> itemPath = nullptr;

  if (!selection.stagedPhotoIndex.empty()) {
    itemPath = mPhotoBook->imageViews().stagedImages().picture(
        selection.stagedPhotoIndex.at(0));
  }
  else if (!selection.unstagedLineIndex.empty()) {
    itemPath = mPhotoBook->imageViews().imageMonitor().image(
        selection.importListIndex.value(), selection.unstagedLineIndex.at(0));
  }

  GalleryLeftButton().IsEnabled(itemPath != nullptr);
  GalleryRightButton().IsEnabled(itemPath != nullptr);

  if (itemPath) {
    GalleryMainText().Text(
        winrt::to_hstring(itemPath->resources().full.filename().string()));
  }
  else if (mPhotoBook->imageViews().imageMonitor().containsRow(
               selectedRootPath)) {
    auto rootName = mPhotoBook->imageViews().imageMonitor().rowPath(
        selection.importListIndex.value());
    GalleryMainText().Text(winrt::to_hstring(rootName.filename().string()));
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
  mPhotoBook->loadProject();
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
  auto projectDetails = mPhotoBook->projectDetails();
  bool alreadySaved = mPhotoBook->persistence()->isSaved(projectDetails);
  if (alreadySaved) {
    return;
  }
  mPopups.fireSaveFilePicker(
      MainWindow::sMainWindowHandle,
      [this](std::variant<std::string, PBDev::Error> result) {
        if (std::holds_alternative<std::string>(result)) {
          auto &newName = std::get<std::string>(result);

          mPhotoBook->savePhotobook(newName);
          mPhotoBook->discardPhotobook();
          Frame().Navigate(winrt::xaml_typename<PhotobookUI::Dashboard>());
        }
        else {
          OnError(std::get<PBDev::Error>(result));
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
        MainWindow::sMainWindowHandle, [this, nativeExportName](Path path) {
          Post([this, path{path}, nativeExportName{nativeExportName}]() {
            mPhotoBook->exportAlbum(nativeExportName, path);
          });
        });
  }
}

void TableContentPage::OnContentDialogDiscardClicked(
    [[maybe_unused]] Windows::Foundation::IInspectable const &,
    [[maybe_unused]] Microsoft::UI::Xaml::Controls::
        ContentDialogButtonClickEventArgs const &)
{
  PB::printDebug("OnContentDialogDiscardClicked\n");
  mPhotoBook->discardPhotobook();

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
