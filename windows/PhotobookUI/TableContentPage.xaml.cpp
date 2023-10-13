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

#include "FirstPage.xaml.h"
#include "MainWindow.xaml.h"

#include <microsoft.ui.xaml.window.h>
#include <winrt/Windows.ApplicationModel.Core.h>
#include <winrt/Windows.Storage.h>
#include <winrt/Windows.UI.Core.h>
#include <winrt/Windows.UI.ViewManagement.h>
#include <winrt/Windows.UI.Xaml.Interop.h>

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

  PaperComboBox().SelectedIndex(0);
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

int TableContentPage::CanvasWidth() { return mCanvasSize.first; }

int TableContentPage::CanvasHeight() { return mCanvasSize.second; }

void TableContentPage::OnImportFolderAdded(IInspectable const &,
                                           RoutedEventArgs const &)
{
  mPopups.fireFolderPicker(
      MainWindow::sMainWindowhandle,
      [this](PB::Path path) { mPhotoBook.addImportFolder(path); });
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

void TableContentPage::OnExitClicked(
    [[maybe_unused]] Windows::Foundation::IInspectable const    &sender,
    [[maybe_unused]] Microsoft::UI::Xaml::RoutedEventArgs const &args)
{
  ProjectExitDialogDisplay();
  mExitFlag = true;
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

  PB::printDebug("Key %d, pressed\n", (int)key);
  switch (key) {
  case Windows::System::VirtualKey::Left: {
    mPhotoBook.gallery().navigateLeft();
    UpdateGalleryLabel();
    break;
  }
  case Windows::System::VirtualKey::Right: {
    mPhotoBook.gallery().navigateRight();
    UpdateGalleryLabel();
    break;
  }
  case Windows::System::VirtualKey::Delete: {
    auto stagedImagesIndex = StagedListView().SelectedIndex();

    if (stagedImagesIndex > -1) {
      mPhotoBook.deleteStagedPhoto({stagedImagesIndex});
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

void TableContentPage::UpdateCanvasSize()
{
  // The new size is computed based on the CanvasBorder size

  int width = (int)CanvasBorder().ActualWidth();
  int height = (int)CanvasBorder().ActualHeight();

  if (width > 0 && height > 0) {

    auto paperSettings = mPhotoBook.paperSettings();

    double ratio = PaperToCanvasRatio(paperSettings.width, paperSettings.height,
                                      width, height);

    auto newWidth = (int)floor((double)width / ratio);
    auto newHeight = (int)floor((double)height / ratio);

    mCanvasSize = {newWidth, newHeight};
  }
}

void TableContentPage::OnTableContentSizeChanged(
    [[maybe_unused]] Windows::Foundation::IInspectable const         &sender,
    [[maybe_unused]] Microsoft::UI::Xaml::SizeChangedEventArgs const &args)
{
  UpdateCanvasSize();
  GalleryCanvas().Invalidate();
}

void TableContentPage::OnPaperComboBoxSelected(
    [[maybe_unused]] Windows::Foundation::IInspectable const &sender,
    [[maybe_unused]] Microsoft::UI::Xaml::Controls::
        SelectionChangedEventArgs const &args)
{
  auto index = PaperComboBox().SelectedIndex();
  PB::basicAssert(index > -1);
  switch (index) {
  case 0: {
    mPhotoBook.setPaperSettings(PB::PaperSettings(PB::A4_PAPER));
    break;
  }
  case 1: {
    mPhotoBook.setPaperSettings(PB::PaperSettings(PB::A5_PAPER));
    break;
  }
  case 2: {
    mPhotoBook.setPaperSettings(PB::PaperSettings(PB::A3_PAPER));
    break;
  }
  case 3: {
    // Start the dialog.
  }
  }
}

void TableContentPage::OnDropIntoStagedPhotos(
    [[maybe_unused]] Windows::Foundation::IInspectable const  &sender,
    [[maybe_unused]] Microsoft::UI::Xaml::DragEventArgs const &args)
{
  PB::printDebug("Drop on staged list view.\n");
  for (auto &image : mDragAndDropSelectedImages) {
    mPhotoBook.addStagedPhoto(image);
  }

  mDragAndDropSelectedImages.clear();
}

void TableContentPage::OnGalleryLeft(
    [[maybe_unused]] Windows::Foundation::IInspectable const    &sender,
    [[maybe_unused]] Microsoft::UI::Xaml::RoutedEventArgs const &args)
{
  mPhotoBook.gallery().navigateLeft();

  UpdateGalleryLabel();
}

void TableContentPage::OnGalleryRight(
    [[maybe_unused]] Windows::Foundation::IInspectable const    &sender,
    [[maybe_unused]] Microsoft::UI::Xaml::RoutedEventArgs const &args)
{
  mPhotoBook.gallery().navigateRight();

  UpdateGalleryLabel();
}

void TableContentPage::CanvasControlDraw(
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
  if (PB::MediaMap::validImagePath(maybeThumbnail->fullPath)) {
    image = mPhotoBook.loadGalleryImage(mediumThumbnailPath.string(),
                                        {portviewWidth, portviewHeight});
  }
  else {
    image = PB::Process::singleColorImage(portviewWidth, portviewHeight,
                                          {255, 255, 255})();

    image = PB::Process::addText({portviewWidth / 2, portviewHeight / 2},
                                 mediumThumbnailPath.filename().string(),
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

void TableContentPage::OnMappingFinished()
{
  mNavigationItemsCollection.Clear();
  auto rootFolders = mPhotoBook.imageSupport().groups();
  for (auto &path : rootFolders) {
    mNavigationItemsCollection.Append(
        winrt::to_hstring(path.first.filename().string()));
  }

  MediaListView().ItemsSource(mNavigationItemsCollection);

  if (!rootFolders.empty()) {
    AddMediaButton().VerticalAlignment(VerticalAlignment::Bottom);
  }
  else {
    AddMediaButton().VerticalAlignment(VerticalAlignment::Center);
  }

  StatusLabelText().Text(winrt::to_hstring("Status: Idle"));
  MainProgressBar().Visibility(
      winrt::Microsoft::UI::Xaml::Visibility::Collapsed);

  PB::printDebug("Index selected %d\n",
                 (int)(mNavigationItemsCollection.Size() - 1));

  MediaListView().SelectedIndex(mNavigationItemsCollection.Size() - 1);
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
  mPhotoBook.gallery().selectImportFolder(index, iterator);

  UpdateGalleryLabel();
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

  mPhotoBook.gallery().selectStagedPhotos(iterator);
  UpdateGalleryLabel();
}

void TableContentPage::OnMappingStopped() {}

void TableContentPage::OnMappingStarted() {}

void TableContentPage::OnMappingPaused() {}

void TableContentPage::OnMappingResumed() {}

void TableContentPage::OnProgressUpdate(int progress, int reference)
{
  MainProgressBar().Maximum(reference);
  MainProgressBar().Value(progress);
  StatusLabelText().Text(winrt::to_hstring("Status: In progress..."));
}

void TableContentPage::OnUnstagedImageAdded(PB::Path fullPath,
                                            PB::Path mediumPath,
                                            PB::Path smallPath, int position)
{
  mUnstagedImageCollection.SetAt(
      position, ImageUIData(winrt::to_hstring(fullPath.string()),
                            winrt::to_hstring(mediumPath.string()),
                            winrt::to_hstring(smallPath.string())));
}

void TableContentPage::OnAddingUnstagedImagePlaceholder(unsigned size)
{
  for (int i = 0; i < (int)size; ++i) {
    mUnstagedImageCollection.Append(ImageUIData());
  }
}

void TableContentPage::OnStagedImageAdded(PB::Thumbnails image, int index)
{
  PB::basicAssert(index == -1);
  if (index == -1) {
    ImageUIData winRTImage(winrt::to_hstring(image.fullPath.string()),
                           winrt::to_hstring(image.mediumThumbnail.string()),
                           winrt::to_hstring(image.smallThumbnail.string()));
    mStagedImageCollection.Append(winRTImage);
  }
}

void TableContentPage::OnStagedImageRemoved(std::vector<int> removedIndexes) {
  std::sort(removedIndexes.begin(), removedIndexes.end(), std::greater<int>());
  for (int i = 0; i < removedIndexes.size(); ++i)
  {
    mStagedImageCollection.RemoveAt(removedIndexes.at(i));
  }
}

void TableContentPage::OnError(PB::Error error) {}

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

  mPhotoBook.loadProject(
      PB::Path(fullPath), [this](std::optional<PB::Error> maybeError) {
        if (maybeError) {
          Frame().Navigate(winrt::xaml_typename<PhotobookUI::FirstPage>());
        }
      });
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
          auto newName = std::get<std::string>(result);

          mPhotoBook.savePhotoBook(newName);
          Frame().Navigate(winrt::xaml_typename<PhotobookUI::FirstPage>());
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

  if (nativeExportName.empty()) {
    PostponeError("The given name must not be empty!");
  }
  else if (mStagedImageCollection.Size() == 0) {
    PostponeError("There is no staged photo!");
  }
  else {
    mPopups.fireFolderPicker(
        MainWindow::sMainWindowhandle, [this, nativeExportName](PB::Path path) {
          std::vector<PB::Path> thumbnailPaths;
          for (auto item : mStagedImageCollection) {
            thumbnailPaths.push_back(winrt::to_string(item.FullPath()));
          }
          mPhotoBook.exporter<PB::Pdf>().exportImages(nativeExportName, path,
                                                      thumbnailPaths);
        });
  }
}

void TableContentPage::OnContentDialogDiscardClicked(
    [[maybe_unused]] Windows::Foundation::IInspectable const &,
    [[maybe_unused]] Microsoft::UI::Xaml::Controls::
        ContentDialogButtonClickEventArgs const &)
{
  mPhotoBook.discardPhotoBook();
  if (mExitFlag) {
    Post([]() { winrt::Microsoft::UI::Xaml::Application::Current().Exit(); });
  }
  else {
    Frame().Navigate(winrt::xaml_typename<PhotobookUI::FirstPage>());
  }
}

void TableContentPage::OnContentDialogCancelClicked(
    [[maybe_unused]] Windows::Foundation::IInspectable const &,
    [[maybe_unused]] Microsoft::UI::Xaml::Controls::
        ContentDialogButtonClickEventArgs const &)
{
}

} // namespace winrt::PhotobookUI::implementation
