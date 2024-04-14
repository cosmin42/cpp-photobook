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

#include <pb/config/Log.h>
#include <pb/image/Image.h>
#include <pb/image/ImageFactory.h>
#include <pb/project/PaperSettings.h>

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
    : mPhotoBook(Application::Current()
                     .as<winrt::PhotobookUI::implementation::App>()
                     ->api())
{
  mListener.configure(this);

  auto photobookListener = dynamic_cast<PB::PhotobookListener *>(&mListener);
  PBDev::basicAssert(photobookListener != nullptr);
  mPhotoBook->configure(photobookListener);

  InitializeComponent();

  mPhotoBook->configure(ScreenSize());

  auto stagedImageListener = dynamic_cast<PB::StagedImagesListener *>(this);
  PBDev::basicAssert(stagedImageListener != nullptr);
  mPhotoBook->configure((PB::StagedImagesListener *)this);

  auto imageMonitorListener = dynamic_cast<PB::ImageMonitorListener *>(this);
  PBDev::basicAssert(imageMonitorListener != nullptr);
  mPhotoBook->configure(imageMonitorListener);

  mNavigationItemsCollection =
      winrt::single_threaded_observable_vector<winrt::hstring>();
  mUnstagedImageCollection =
      winrt::single_threaded_observable_vector<ImageUIData>();
  mStagedImageCollection =
      winrt::single_threaded_observable_vector<ImageUIData>();

  MainWindow::sMainExitFunction = [this]() {
    auto isSaved = mPhotoBook->project()->isSaved(
        mPhotoBook->imageViews().imageMonitor().unstaged(),
        mPhotoBook->imageViews().stagedImages().stagedPhotos(),
        mPhotoBook->imageViews().imageMonitor().rowList());

    if (isSaved) {
      Post([]() { winrt::Microsoft::UI::Xaml::Application::Current().Exit(); });
    }
    else {
      mExitFlag = true;
      SaveProjectDialogDisplay();
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
  auto paperSettings = mPhotoBook->project()->currentProject()->paperSettings;
  PBDev::basicAssert(
      mPhotoBook->project()->currentProject()->paperSettings.ppi > 0);

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
  auto paperSettings = mPhotoBook->project()->currentProject()->paperSettings;
  PBDev::basicAssert(
      mPhotoBook->project()->currentProject()->paperSettings.ppi > 0);

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
  mPopups.fireFolderPicker(MainWindow::sMainWindowHandle, [this](Path path) {
    mPhotoBook->addImportFolder(path);
  });
}

void TableContentPage::OnImportFolderRemoved(IInspectable const &,
                                             RoutedEventArgs const &)
{
  auto selectedIndex = MediaListView().SelectedIndex();

  PBDev::basicAssert(selectedIndex >= 0);

  auto rowPath = mPhotoBook->imageViews().imageMonitor().rowPath(selectedIndex);

  mPhotoBook->removeImportFolder(rowPath);
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
  auto isSaved = mPhotoBook->project()->isSaved(
      mPhotoBook->imageViews().imageMonitor().unstaged(),
      mPhotoBook->imageViews().stagedImages().stagedPhotos(),
      mPhotoBook->imageViews().imageMonitor().rowList());
  if (!isSaved) {
    mBackFlag = true;
    SaveProjectDialogDisplay();
  }
  else {
    mPhotoBook->unloadProject();
    Frame().Navigate(winrt::xaml_typename<PhotobookUI::Dashboard>());
  }
}

void TableContentPage::OnAboutClicked(
    [[maybe_unused]] Windows::Foundation::IInspectable const    &sender,
    [[maybe_unused]] Microsoft::UI::Xaml::RoutedEventArgs const &args)
{
  GenericMessageTextBlock().Text(
      winrt::to_hstring("Version: " + std::string(PB::Context::VERSION) +
                        "\nEmail: ionut.cosmin.mihai@gmail.com"));
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
  // TODO: Raname the blow stupid repetition
  mPhotoBook->project()->save(
      boost::uuids::to_string(mPhotoBook->project()->currentProjectUUID()),
      mPhotoBook->imageViews().imageMonitor().unstaged(),
      mPhotoBook->imageViews().stagedImages().stagedPhotos(),
      mPhotoBook->imageViews().imageMonitor().rowList());
}

void TableContentPage::OnSaveAsClicked(
    [[maybe_unused]] Windows::Foundation::IInspectable const    &sender,
    [[maybe_unused]] Microsoft::UI::Xaml::RoutedEventArgs const &args)
{
  RenameProjectDialogDisplay();
}

void TableContentPage::OnNewClicked(
    [[maybe_unused]] Windows::Foundation::IInspectable const    &sender,
    [[maybe_unused]] Microsoft::UI::Xaml::RoutedEventArgs const &args)
{
  auto isSaved = mPhotoBook->project()->isSaved(
      mPhotoBook->imageViews().imageMonitor().unstaged(),
      mPhotoBook->imageViews().stagedImages().stagedPhotos(),
      mPhotoBook->imageViews().imageMonitor().rowList());
  if (isSaved) {
    mPhotoBook->unloadProject();
    Frame().Navigate(winrt::xaml_typename<PhotobookUI::Dashboard>(),
                     winrt::box_value(winrt::to_hstring("new-project")));
  }
  else {
    mNewProjectFlag = true;
    SaveProjectDialogDisplay();
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

    auto selection = SelectionIndex();
    OnStagedImageRemoved(selection.stagedPhotoIndex);

    mPhotoBook->imageViews().stagedImages().removePicture(
        selection.stagedPhotoIndex);
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
  bool allowDrag = true;
  for (auto item : args.Items()) {
    auto image = item.as<ImageUIData>();
    if (!image.Processed()) {
      allowDrag = false;
      break;
    }
    auto keyPath = winrt::to_string(image.KeyPath());
    auto virtualImage = mPhotoBook->imageViews().imageMonitor().image(keyPath);

    mDragAndDropSelectedImages.push_back(virtualImage);
  }
  if (allowDrag) {
    mDragSource = DragSource::Unstaged;
  }
  else {
    mDragSource = DragSource::None;
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
  // TODO: ???
  auto framework = args.OriginalSource().as<FrameworkElement>();

  auto image = framework.DataContext().as<ImageUIData>();
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
    auto paperSettings = mPhotoBook->project()->currentProject()->paperSettings;

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
  if (mDragSource == DragSource::None) {
    return;
  }
  PB::printDebug("Drop on staged list view.\n");

  // WORKAROUND BEGIN
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
  // WORKAROUND END

  std::vector<std::shared_ptr<PB::VirtualImage>> copyOfDraggedImages;

  for (int i = 0; i < (int)mDragAndDropSelectedImages.size(); ++i) {
    copyOfDraggedImages.push_back(
        PB::ImageFactory::inst().copyImage(mDragAndDropSelectedImages.at(i)));
  }

  mPhotoBook->imageViews().stagedImages().addPictures(copyOfDraggedImages,
                                                      insertPostion);

  OnStagedImageAdded(copyOfDraggedImages, insertPostion);

  mDragAndDropSelectedImages.clear();
}

void TableContentPage::Right()
{
  auto selection = SelectionIndex();

  int nextIndex = 0;

  if (!selection.unstagedLineIndex.empty()) {
    PBDev::basicAssert(selection.unstagedLineIndex.size() == 1);

    nextIndex = (selection.unstagedLineIndex.at(0) + 1) %
                (int)mUnstagedImageCollection.Size();

    UnstagedListView().DeselectRange(Microsoft::UI::Xaml::Data::ItemIndexRange(
        0, mUnstagedImageCollection.Size()));
    UnstagedListView().SelectRange({nextIndex, 1});
  }
  else if (!selection.stagedPhotoIndex.empty()) {
    PBDev::basicAssert(selection.stagedPhotoIndex.size() == 1);
    nextIndex = (selection.stagedPhotoIndex.at(0) + 1) %
                (int)mStagedImageCollection.Size();
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

  if (selection.stagedPhotoIndex.empty() &&
      selection.unstagedLineIndex.empty()) {
    session.Clear({0, 0, 0});
    return;
  }

  int32_t portviewWidth = (int32_t)GalleryCanvas().ActualWidth();

  int32_t portviewHeight = (int32_t)GalleryCanvas().ActualHeight();

  std::shared_ptr<cv::Mat> image = nullptr;

  std::shared_ptr<PB::VirtualImage> imagePtr = nullptr;

  if (!selection.unstagedLineIndex.empty()) {
    imagePtr = mPhotoBook->imageViews().imageMonitor().image(
        selection.importListIndex.value(), selection.unstagedLineIndex.at(0));
  }
  else if (!selection.stagedPhotoIndex.empty()) {
    imagePtr = mPhotoBook->imageViews().stagedImages().picture(
        selection.stagedPhotoIndex.at(0));
  }

  PBDev::basicAssert(imagePtr != nullptr);

  auto mediumThumbnailPath = imagePtr->frontend().medium;

  PBDev::basicAssert(!mediumThumbnailPath.empty());

  // Draw the image

  image = PB::Process::singleColorImage(portviewWidth, portviewHeight,
                                        {255, 255, 255})();

  auto tmpImage = PB::ImageReader().read(mediumThumbnailPath.string(),
                                         {portviewWidth, portviewHeight});

  PB::Process::overlap(tmpImage, PB::Process::alignToCenter())(image);

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

void TableContentPage::OnThumbnailsProcessingFinished(Path rootPath)
{
  mLoadedFinishedImportFolders.insert(rootPath);

  auto selection = SelectionIndex();

  auto selectedIndex = selection.importListIndex;
  auto rowPath =
      mPhotoBook->imageViews().imageMonitor().rowPath(selectedIndex.value());

  if (rowPath == rootPath) {
    UpdateUnstagedPhotoLine();
  }

  PB::printDebug("Index selected %d\n",
                 (int)(mNavigationItemsCollection.Size() - 1));
}

void TableContentPage::OnImportSelectionChanged(
    [[maybe_unused]] ::winrt::Windows::Foundation::IInspectable const &,
    [[maybe_unused]] ::winrt::Microsoft::UI::Xaml::Controls::
        SelectionChangedEventArgs const &)
{
  UpdateUnstagedPhotoLine();

  if (UnstagedListView().Items().Size() > 0) {
    UnstagedListView().SelectRange({0, 1});
  }
  else if (StagedListView().Items().Size() > 0) {
    StagedListView().SelectRange({0, 1});
  }
}

void TableContentPage::OnStagedImageCollectionChanged(
    [[maybe_unused]] IObservableVector<ImageUIData> const    &sender,
    winrt::Windows::Foundation::Collections::CollectionChange changeType,
    int                                                       changedIndex)
{
  if (mDragSource != DragSource::Staged) {
    return;
  }
  if (changeType ==
      winrt::Windows::Foundation::Collections::CollectionChange::ItemInserted) {
    mPhotoBook->imageViews().stagedImages().popImages(changedIndex);
  }
  else if (changeType == winrt::Windows::Foundation::Collections::
                             CollectionChange::ItemRemoved) {
    mPhotoBook->imageViews().stagedImages().stashImages(
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

// TODO: Remove this
void TableContentPage::UpdateUnstagedImagesView(int index)
{
  auto iterator =
      mPhotoBook->imageViews().imageMonitor().statefulIterator((unsigned)index);

  if (!iterator.valid()) {
    return;
  }

  auto rootPath = mPhotoBook->imageViews().imageMonitor().rowPath(index);

  for (int i = 0;
       i < (int)mPhotoBook->imageViews().imageMonitor().rowSize(index); ++i) {
    auto virtualImage = mPhotoBook->imageViews().imageMonitor().image(index, i);
    mUnstagedImageCollection.SetAt(
        i,
        ImageUIData(winrt::to_hstring(virtualImage->keyPath().string()),
                    winrt::to_hstring(virtualImage->frontend().full.string()),
                    winrt::to_hstring(virtualImage->frontend().medium.string()),
                    winrt::to_hstring(virtualImage->frontend().small.string()),
                    virtualImage->processed()));
  }
}

void TableContentPage::UpdateUnstagedImage(int row, int index)
{
  auto selection = SelectionIndex();

  auto importSelectedIndex = selection.importListIndex;

  if (importSelectedIndex.has_value() &&
      (int)importSelectedIndex.value() == row) {
    auto virtualImage =
        mPhotoBook->imageViews().imageMonitor().image(row, index);

    mUnstagedImageCollection.SetAt(
        index,
        ImageUIData(winrt::to_hstring(virtualImage->keyPath().string()),
                    winrt::to_hstring(virtualImage->frontend().full.string()),
                    winrt::to_hstring(virtualImage->frontend().medium.string()),
                    winrt::to_hstring(virtualImage->frontend().small.string()),
                    virtualImage->processed()));
  }
}

void TableContentPage::OnUnstagedPhotosSelectionChanged(
    [[maybe_unused]] ::winrt::Windows::Foundation::IInspectable const &,
    [[maybe_unused]] ::winrt::Microsoft::UI::Xaml::Controls::
        SelectionChangedEventArgs const &)
{
  if (!mLinesExclusiveSelection) {
    mLinesExclusiveSelection = true;
    StagedListView().DeselectRange(Microsoft::UI::Xaml::Data::ItemIndexRange(
        0, mStagedImageCollection.Size()));
    UpdateGalleryLabel();
    UpdateGallery();
  }
  mLinesExclusiveSelection = false;
}

void TableContentPage::OnStagedPhotosSelectionChanged(
    [[maybe_unused]] ::winrt::Windows::Foundation::IInspectable const &,
    [[maybe_unused]] ::winrt::Microsoft::UI::Xaml::Controls::
        SelectionChangedEventArgs const &)
{
  if (!mLinesExclusiveSelection) {
    mLinesExclusiveSelection = true;
    UnstagedListView().DeselectRange(Microsoft::UI::Xaml::Data::ItemIndexRange(
        0, mUnstagedImageCollection.Size()));

    UpdateGalleryLabel();
    UpdateGallery();
  }
  mLinesExclusiveSelection = false;
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

void TableContentPage::UpdateStatusBar(PB::ProgressInfo defined,
                                       PB::ProgressInfo undefined)
{
  if (defined.jobsProgress.empty() && undefined.jobsProgress.empty()) {
    StatusLabelText().Text(winrt::to_hstring("Status: Idle."));
  }
  else {
    std::string concatenatedStrings;

    for (auto const &name : defined.jobsProgress) {
      concatenatedStrings =
          std::string(magic_enum::enum_name(defined.jobType)) + " " +
          concatenatedStrings + name + ", ";
    }

    for (auto const &name : undefined.jobsProgress) {
      concatenatedStrings =
          std::string(magic_enum::enum_name(undefined.jobType)) + " " +
          concatenatedStrings + name + "(" +
          std::to_string(undefined.progress) + ")" + ", ";
    }

    StatusLabelText().Text(winrt::to_hstring(concatenatedStrings));
  }
}
void TableContentPage::OnMappingAborted(Path path) {}

void TableContentPage::OnMappingStarted(Path path) {}

void TableContentPage::OnProjectRenamed() {}

void TableContentPage::OnMappingFinished(Path path)
{
  mNavigationItemsCollection.Append(
      winrt::to_hstring(path.filename().string()));

  MediaListView().ItemsSource(mNavigationItemsCollection);

  MediaListView().SelectedIndex(mNavigationItemsCollection.Size() - 1);
}

void TableContentPage::onProgressUpdate(PB::ProgressInfo definedProgress,
                                        PB::ProgressInfo undefinedProgress)
{
  if (definedProgress.jobsProgress.empty() ||
      definedProgress.progressType == PB::ProgressType::None) {
    MainProgressBar().Visibility(
        winrt::Microsoft::UI::Xaml::Visibility::Collapsed);
  }
  else {
    MainProgressBar().Visibility(
        winrt::Microsoft::UI::Xaml::Visibility::Visible);
    MainProgressBar().Maximum(definedProgress.progressCap);
    MainProgressBar().Value(definedProgress.progress);
  }

  UpdateStatusBar(definedProgress, undefinedProgress);
}

void TableContentPage::OnExportProgressUpdate(int progress, int reference)
{
  MainProgressBar().Visibility(winrt::Microsoft::UI::Xaml::Visibility::Visible);
  MainProgressBar().Maximum(reference);
  MainProgressBar().Value(progress);
}

void TableContentPage::OnExportFinished()
{
  MainProgressBar().Visibility(
      winrt::Microsoft::UI::Xaml::Visibility::Collapsed);
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
          winrt::to_hstring(photo->keyPath().string()),
          winrt::to_hstring(photo->frontend().full.string()),
          winrt::to_hstring(photo->frontend().medium.string()),
          winrt::to_hstring(photo->frontend().small.string()),
          photo->processed());
      mStagedImageCollection.Append(winRTImage);
    }
  }
  else if (index < (int)mStagedImageCollection.Size()) {
    for (auto photo : photos) {
      ImageUIData winRTImage(
          winrt::to_hstring(photo->keyPath().string()),
          winrt::to_hstring(photo->frontend().full.string()),
          winrt::to_hstring(photo->frontend().medium.string()),
          winrt::to_hstring(photo->frontend().small.string()),
          photo->processed());
      mStagedImageCollection.InsertAt(index, winRTImage);
    }
  }
}

void TableContentPage::OnStagedImageRemoved(
    std::vector<unsigned> removedIndexes)
{
  std::sort(removedIndexes.begin(), removedIndexes.end(), std::greater<int>());
  for (int i = 0; i < removedIndexes.size(); ++i) {
      // TODO: Remove path
    auto path = winrt::to_string(
        mStagedImageCollection.GetAt(removedIndexes.at(i)).FullPath());
    mStagedImageCollection.RemoveAt(removedIndexes.at(i));
  }

  UpdateGallery();
}

void TableContentPage::OnError(PBDev::Error error)
{
  auto errorString = std::string(magic_enum::enum_name(error.kind()));
  GenericErrorTextBlock().Text(winrt::to_hstring("Code: " + errorString + "."));
  GenericErrorDialogDisplay();
}

void TableContentPage::Post(std::function<void()> f)
{
  bool success = MainWindow::sMainThreadDispatcher.TryEnqueue(
      DispatcherQueuePriority::Normal, [f{f}]() { f(); });
  PBDev::basicAssert(success);
}

void TableContentPage::onPicturesAdded(int index, int size)
{
  UNUSED(index);
  UNUSED(size);
}

void TableContentPage::onPictureRemoved(std::vector<unsigned> index)
{
  if (index.empty() ||
      mPhotoBook->imageViews().stagedImages().stagedPhotos().empty()) {
    return;
  }
  auto selection = SelectionIndex();

  if (selection.stagedPhotoIndex.empty()) {
    auto newSelectionIndex = std::max<int>((int)index.at(0) - 1, 0);

    StagedListView().SelectRange({newSelectionIndex, 1});
  }
}

void TableContentPage::onImportFolderAdded() {}

void TableContentPage::LoadImages()
{
  auto rowList = mPhotoBook->imageViews().imageMonitor().rowList();

  for (int i = 0; i < rowList.size(); ++i) {
    mNavigationItemsCollection.Append(
        winrt::to_hstring(rowList.at(i).filename().string()));
  }

  MediaListView().ItemsSource(mNavigationItemsCollection);
  if (!rowList.empty()) {
    MediaListView().SelectedIndex(mNavigationItemsCollection.Size() - 1);

    auto lastRowIndex = (int)(rowList.size() - 1);
    auto rootPath =
        mPhotoBook->imageViews().imageMonitor().rowPath(lastRowIndex);

    for (int i = 0;
         i < (int)mPhotoBook->imageViews().imageMonitor().rowSize(lastRowIndex);
         ++i) {
      auto virtualImage =
          mPhotoBook->imageViews().imageMonitor().image(lastRowIndex, i);
      mUnstagedImageCollection.SetAt(
          i, ImageUIData(
                 winrt::to_hstring(virtualImage->keyPath().string()),
                 winrt::to_hstring(virtualImage->frontend().full.string()),
                 winrt::to_hstring(virtualImage->frontend().medium.string()),
                 winrt::to_hstring(virtualImage->frontend().small.string()),
                 virtualImage->processed()));
    }
  }

  auto stagedPictures = mPhotoBook->imageViews().stagedImages().stagedPhotos();

  for (int i = 0; i < (int)stagedPictures.size(); ++i) {
    mStagedImageCollection.Append(ImageUIData(
        winrt::to_hstring(stagedPictures.at(i)->keyPath().string()),
        winrt::to_hstring(stagedPictures.at(i)->frontend().full.string()),
        winrt::to_hstring(stagedPictures.at(i)->frontend().medium.string()),
        winrt::to_hstring(stagedPictures.at(i)->frontend().small.string()),
        stagedPictures.at(i)->processed()));
  }
}

void TableContentPage::onRefresh() { LoadImages(); }

void TableContentPage::UpdateUnstagedLine()
{
  // TODO: Improvements can be done here (clearing is not always necessary)
  auto selection = SelectionIndex();
  mUnstagedImageCollection.Clear();
  if (selection.importListIndex) {
    int index = selection.importListIndex.value();
    for (int i = 0;
         i < (int)mPhotoBook->imageViews().imageMonitor().rowSize(index); ++i) {
      auto virtualImage =
          mPhotoBook->imageViews().imageMonitor().image(index, i);
      mUnstagedImageCollection.Append(ImageUIData(
          winrt::to_hstring(virtualImage->keyPath().string()),
          winrt::to_hstring(virtualImage->frontend().full.string()),
          winrt::to_hstring(virtualImage->frontend().medium.string()),
          winrt::to_hstring(virtualImage->frontend().small.string()),
          virtualImage->processed()));
    }
  }
}

void TableContentPage::onImportFolderRemoved(unsigned index)
{
  MediaListView().DeselectRange(Microsoft::UI::Xaml::Data::ItemIndexRange(
      0, mNavigationItemsCollection.Size()));
  mNavigationItemsCollection.RemoveAt(index);
  MediaListView().ItemsSource(mNavigationItemsCollection);
  if (mNavigationItemsCollection.Size() > 0) {
    MediaListView().SelectedIndex(0);
  }

  UpdateUnstagedLine();
}

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

void TableContentPage::UpdateUnstagedPhotoLine()
{
  auto selection = SelectionIndex();

  if (selection.importListIndex) {
    auto iterator = mPhotoBook->imageViews().imageMonitor().statefulIterator(
        selection.importListIndex.value());

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

      for (int i = 0; i < (int)iterator.size(); ++i) {
        auto virtualImage = iterator[i].current();
        mUnstagedImageCollection.SetAt(
            i, ImageUIData(
                   winrt::to_hstring(virtualImage->keyPath().string()),
                   winrt::to_hstring(virtualImage->frontend().full.string()),
                   winrt::to_hstring(virtualImage->frontend().medium.string()),
                   winrt::to_hstring(virtualImage->frontend().small.string()),
                   virtualImage->processed()));
      }
    }
    else {
      mUnstagedImageCollection.Clear();
    }
  }
}

void TableContentPage::UpdateGallery() { GalleryCanvas().Invalidate(); }

void TableContentPage::UpdateGalleryLabel()
{
  auto selection = SelectionIndex();

  std::shared_ptr<PB::VirtualImage> itemPath = nullptr;

  if (!selection.stagedPhotoIndex.empty()) {
    itemPath = mPhotoBook->imageViews().stagedImages().picture(
        selection.stagedPhotoIndex.at(0));
  }
  else if (!selection.unstagedLineIndex.empty()) {
    itemPath = mPhotoBook->imageViews().imageMonitor().image(
        selection.importListIndex.value(), selection.unstagedLineIndex.at(0));
  }
  else {
    GalleryLeftButton().IsEnabled(false);
    GalleryRightButton().IsEnabled(false);
    GalleryMainText().Text(winrt::to_hstring("Nothing selected."));
    return;
  }

  GalleryMainText().Text(
      winrt::to_hstring(itemPath->keyPath().stem().string()));

  // TODO: Make this better
  GalleryLeftButton().IsEnabled(itemPath != nullptr);
  GalleryRightButton().IsEnabled(itemPath != nullptr);
}

void TableContentPage::PostponeError(std::string message)
{
  GenericErrorTextBlock().Text(winrt::to_hstring(message));
  Post([this]() { GenericErrorDialogDisplay(); });
}

void TableContentPage::OnNavigatedTo(
    Microsoft::UI::Xaml::Navigation::NavigationEventArgs e)
{
  LoadImages();
  if (UnstagedListView().Items().Size() > 0) {
    UnstagedListView().SelectRange({0, 1});
  }
  else if (StagedListView().Items().Size() > 0) {
    StagedListView().SelectRange({0, 1});
  }
}

void TableContentPage::OnExportClicked(
    [[maybe_unused]] Windows::Foundation::IInspectable const &,
    [[maybe_unused]] Microsoft::UI::Xaml::RoutedEventArgs const &)
{
  if (mStagedImageCollection.Size() == 0) {
    PostponeError("There is no staged photo!");
  }
  else {
    ExportDialogDisplay();
  }
}

void TableContentPage::OnExportContentDialogClicked(
    [[maybe_unused]] Windows::Foundation::IInspectable const &sender,
    [[maybe_unused]] Microsoft::UI::Xaml::Controls::
        ContentDialogButtonClickEventArgs const &args)
{
  auto                     exportName = ExportNameTextBox().Text();
  std::string              nativeExportName = winrt::to_string(exportName);
  std::set<PB::ExportType> exportSelection;

  if (OptionPDFCheckBox().IsChecked().as<bool>()) {
    exportSelection.insert(PB::ExportType::Pdf);
  }
  if (OptionJPGCheckBox().IsChecked().as<bool>()) {
    exportSelection.insert(PB::ExportType::Jpg);
  }
  if (OptionPDFLibharuCheckBox().IsChecked().as<bool>()) {
    exportSelection.insert(PB::ExportType::PdfLibharu);
  }
  if (exportSelection.empty()) {
    PostponeError("Check a format!");
    return;
  }

  if (nativeExportName.empty()) {
    PostponeError("The given name must not be empty!");
  }
  else {
    if (!exportSelection.empty()) {
      mPopups.fireFolderPicker(
          MainWindow::sMainWindowHandle,
          [this, nativeExportName,
           exportSelection{exportSelection}](Path path) {
            Post([this, path{path}, nativeExportName{nativeExportName},
                  exportSelection{exportSelection}]() {
              if (exportSelection.contains(PB::ExportType::Pdf)) {
                mPhotoBook->exportPDFAlbum(nativeExportName, path);
              }
              if (exportSelection.contains(PB::ExportType::Jpg)) {
                mPhotoBook->exportJPGAlbum(nativeExportName, path);
              }
              if (exportSelection.contains(PB::ExportType::PdfLibharu)) {
                mPhotoBook->exportPDFLibharu(nativeExportName, path);
              }
            });
          });
    }
  }
}

void TableContentPage::OnContentDialogDiscardClicked(
    [[maybe_unused]] Windows::Foundation::IInspectable const &,
    [[maybe_unused]] Microsoft::UI::Xaml::Controls::
        ContentDialogButtonClickEventArgs const &)
{
  PB::printDebug("OnContentDialogDiscardClicked\n");
  mPhotoBook->unloadProject();

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

// TODO: Add project name validation
void TableContentPage::OnRenameProjectDialogRename(
    [[maybe_unused]] Windows::Foundation::IInspectable const &sender,
    [[maybe_unused]] Microsoft::UI::Xaml::Controls::
        ContentDialogButtonClickEventArgs const &args)
{
  mPhotoBook->project()->rename(
      winrt::to_string(RenameProjectDialogTextBox().Text()));

  if (mExitFlag) {
    Post([]() { winrt::Microsoft::UI::Xaml::Application::Current().Exit(); });
  }
}

void TableContentPage::OnRenameProjectDialogCancel(
    [[maybe_unused]] Windows::Foundation::IInspectable const &sender,
    [[maybe_unused]] Microsoft::UI::Xaml::Controls::
        ContentDialogButtonClickEventArgs const &args)
{
  if (mExitFlag) {
    Post([]() { winrt::Microsoft::UI::Xaml::Application::Current().Exit(); });
  }
}

auto TableContentPage::RenameProjectDialogDisplay() -> winrt::fire_and_forget
{
  co_await RenameProjectDialog().ShowAsync();
}

auto TableContentPage::SaveProjectDialogDisplay() -> winrt::fire_and_forget
{
  co_await SaveProjectDialog().ShowAsync();
}

void TableContentPage::OnSaveProject(
    [[maybe_unused]] Windows::Foundation::IInspectable const &sender,
    [[maybe_unused]] Microsoft::UI::Xaml::Controls::
        ContentDialogButtonClickEventArgs const &args)
{
  mPhotoBook->project()->save(
      boost::uuids::to_string(mPhotoBook->project()->currentProjectUUID()),
      mPhotoBook->imageViews().imageMonitor().unstaged(),
      mPhotoBook->imageViews().stagedImages().stagedPhotos(),
      mPhotoBook->imageViews().imageMonitor().rowList());
  if (mExitFlag) {
    Post([]() { winrt::Microsoft::UI::Xaml::Application::Current().Exit(); });
  }
  if (mNewProjectFlag) {
    mNewProjectFlag = false;
    mPhotoBook->unloadProject();
    Frame().Navigate(winrt::xaml_typename<PhotobookUI::Dashboard>(),
                     winrt::box_value(winrt::to_hstring("new-project")));
  }
  if (mBackFlag) {
    mBackFlag = false;
    mPhotoBook->unloadProject();
    Frame().Navigate(winrt::xaml_typename<PhotobookUI::Dashboard>());
  }
}

void TableContentPage::OnCancelSavingProject(
    [[maybe_unused]] Windows::Foundation::IInspectable const &sender,
    [[maybe_unused]] Microsoft::UI::Xaml::Controls::
        ContentDialogButtonClickEventArgs const &args)
{
  if (mExitFlag) {
    Post([]() { winrt::Microsoft::UI::Xaml::Application::Current().Exit(); });
  }
  if (mNewProjectFlag) {
    mNewProjectFlag = false;
    mPhotoBook->unloadProject();
    Frame().Navigate(winrt::xaml_typename<PhotobookUI::Dashboard>(),
                     winrt::box_value(winrt::to_hstring("new-project")));
  }
  if (mBackFlag) {
    mBackFlag = false;
    mPhotoBook->unloadProject();
    Frame().Navigate(winrt::xaml_typename<PhotobookUI::Dashboard>());
  }
}

} // namespace winrt::PhotobookUI::implementation
