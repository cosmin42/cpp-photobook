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
#include <Shobjidl.h>

#include <winrt/Windows.Storage.Pickers.h>
#include <winrt/Windows.UI.Popups.h>
#include <winrt/Windows.UI.Xaml.Interop.h>

#include <microsoft.ui.xaml.window.h>

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

TableContentPage::TableContentPage()
    : mListener(std::ref(*this)), mPhotoBook(mListener)
{
  mMediaListItemsCollection =
      winrt::single_threaded_observable_vector<winrt::hstring>();
  mUnstagingImageCollection =
      winrt::single_threaded_observable_vector<ImageUIData>();
  mStagedImageCollection =
      winrt::single_threaded_observable_vector<ImageUIData>();
  InitializeComponent();

  UnstagedListView().ItemsSource(mUnstagingImageCollection);
  StagedListView().ItemsSource(mStagedImageCollection);

  KeyUp([this](Windows::Foundation::IInspectable const              &sender,
               Microsoft::UI::Xaml::Input::KeyRoutedEventArgs const &arg) {
    onKeyPressed(sender, arg);
  });

  PaperComboBox().SelectedIndex(0);
}

int TableContentPage::CanvasWidth()
{
  auto paperSettings = mPhotoBook.paperSettings();
  assert(mPhotoBook.paperSettings().ppi > 0);
  return (int)((float)paperSettings.width * (60.0 / (float)paperSettings.ppi));
}

int TableContentPage::CanvasHeight()
{
  auto paperSettings = mPhotoBook.paperSettings();
  assert(mPhotoBook.paperSettings().ppi > 0);
  return (int)((float)paperSettings.height * (60.0 / (float)paperSettings.ppi));
}

auto TableContentPage::fireFolderPicker(
    HWND hWnd, std::function<void(std::string)> onSuccess)
    -> winrt::fire_and_forget
{
  Windows::Storage::Pickers::FolderPicker folderPicker;

  auto initializeWithWindow{folderPicker.as<::IInitializeWithWindow>()};
  initializeWithWindow->Initialize(hWnd);

  folderPicker.FileTypeFilter().Append(L"*");
  auto folder{co_await folderPicker.PickSingleFolderAsync()};

  if (folder) {
    onSuccess(winrt::to_string(folder.Path()));
  }
}

auto TableContentPage::fireSaveFilePicker(
    HWND                                                      hWnd,
    std::function<void(std::variant<std::string, PB::Error>)> onReturn)
    -> winrt::fire_and_forget
{
  Windows::Storage::Pickers::FileSavePicker fileSavePicker;

  auto initializeWithWindow{fileSavePicker.as<::IInitializeWithWindow>()};
  initializeWithWindow->Initialize(hWnd);

  auto plainTextExtensions = winrt::single_threaded_vector<winrt::hstring>();
  plainTextExtensions.Append(winrt::param::hstring(L".photobook"));

  fileSavePicker.SuggestedStartLocation(PickerLocationId::DocumentsLibrary);
  fileSavePicker.FileTypeChoices().Insert(L"Text File", plainTextExtensions);
  fileSavePicker.DefaultFileExtension(L".photobook");
  fileSavePicker.SuggestedFileName(L"Untitled");

  auto filename{co_await fileSavePicker.PickSaveFileAsync()};

  if (filename) {
    onReturn(winrt::to_string(filename.Path()));
  }
  else {
    onReturn(PB::Error() << PB::ErrorCode::CannotSaveFile);
  }
}

void TableContentPage::onAddMediaButtonClicked(IInspectable const &,
                                               RoutedEventArgs const &)
{
  fireFolderPicker(MainWindow::sMainWindowhandle,
                   [this](std::string path) { mPhotoBook.addMedia(path); });
}

auto TableContentPage::projectExitDialogDisplay() -> winrt::fire_and_forget
{
  co_await ProjectExitDialog().ShowAsync();
}

auto TableContentPage::exportDialogDisplay() -> winrt::fire_and_forget
{
  co_await ExportContentDialog().ShowAsync();
}

auto TableContentPage::genericErrorDialogDisplay() -> winrt::fire_and_forget
{
  co_await GenericErrorDialog().ShowAsync();
}

void TableContentPage::onBackClicked(IInspectable const &,
                                     RoutedEventArgs const &)
{
  projectExitDialogDisplay();
}

void TableContentPage::onKeyPressed(
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
    updateGalleryLabel();
    break;
  }
  case Windows::System::VirtualKey::Right: {
    mPhotoBook.gallery().navigateRight();
    updateGalleryLabel();
    break;
  }
  default: {
  }
  }
}

void TableContentPage::OnUnstagedListDragStarted(
    [[maybe_unused]] Windows::Foundation::IInspectable const &sender,
    [[maybe_unused]] Microsoft::UI::Xaml::Controls::
        DragItemsStartingEventArgs const &args)
{
  PB::printDebug("Drag started\n");
  auto selectedRanges =
      sender.as<Microsoft::UI::Xaml::Controls::ListView>().SelectedRanges();

  for (auto range : selectedRanges) {
    for (auto i = range.FirstIndex(); i <= range.LastIndex(); ++i) {
      mDragAndDropSelectedIndexes.push_back(i);
    }
  }
}

void TableContentPage::OnDragOverStagedListView(
    [[maybe_unused]] Windows::Foundation::IInspectable const &sender,
    Microsoft::UI::Xaml::DragEventArgs const                 &args)
{
  args.AcceptedOperation(
      Windows::ApplicationModel::DataTransfer::DataPackageOperation::Copy);
}

void TableContentPage::OnTableContentSizeChanged(
    [[maybe_unused]] Windows::Foundation::IInspectable const         &sender,
    [[maybe_unused]] Microsoft::UI::Xaml::SizeChangedEventArgs const &args)
{
  GalleryCanvas().Invalidate();
}

void TableContentPage::OnPaperComboBoxSelected(
    [[maybe_unused]] Windows::Foundation::IInspectable const &sender,
    [[maybe_unused]] Microsoft::UI::Xaml::Controls::
        SelectionChangedEventArgs const &args)
{
  auto index = PaperComboBox().SelectedIndex();
  assert(index > -1);
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

void TableContentPage::OnDropIntoStagedListView(
    [[maybe_unused]] Windows::Foundation::IInspectable const  &sender,
    [[maybe_unused]] Microsoft::UI::Xaml::DragEventArgs const &args)
{
  PB::printDebug("Drop on staged list view.\n");
  for (auto index : mDragAndDropSelectedIndexes) {
    mStagedImageCollection.Append(mUnstagingImageCollection.GetAt(index));
    auto fullPath =
        winrt::to_string(mUnstagingImageCollection.GetAt(index).FullPath());
    auto mediumPath =
        winrt::to_string(mUnstagingImageCollection.GetAt(index).MediumPath());
    auto smallPath =
        winrt::to_string(mUnstagingImageCollection.GetAt(index).SmallPath());
    mPhotoBook.addStagedPhoto(PB::Thumbnails(
        PB::Path(fullPath), PB::Path(mediumPath), PB::Path(smallPath)));
  }

  mDragAndDropSelectedIndexes.clear();
}

void TableContentPage::onGalleryLeft(
    [[maybe_unused]] Windows::Foundation::IInspectable const    &sender,
    [[maybe_unused]] Microsoft::UI::Xaml::RoutedEventArgs const &args)
{
  mPhotoBook.gallery().navigateLeft();

  updateGalleryLabel();
}

void TableContentPage::onGalleryRight(
    [[maybe_unused]] Windows::Foundation::IInspectable const    &sender,
    [[maybe_unused]] Microsoft::UI::Xaml::RoutedEventArgs const &args)
{
  mPhotoBook.gallery().navigateRight();

  updateGalleryLabel();
}

void TableContentPage::CanvasControlDraw(
    [[maybe_unused]] winrt::Microsoft::Graphics::Canvas::UI::Xaml::
        CanvasControl const &sender,
    winrt::Microsoft::Graphics::Canvas::UI::Xaml::CanvasDrawEventArgs const
        &args)
{
  auto session = args.DrawingSession();

  auto &gallery = mPhotoBook.gallery();

  int32_t portviewWidth = (int32_t)GalleryCanvas().Width();

  int32_t portviewHeight = (int32_t)GalleryCanvas().Height();

  std::shared_ptr<cv::Mat> image = nullptr;

  auto maybeMediumThumbnailPath = gallery.selectedItem();

  if (!maybeMediumThumbnailPath) {
    return;
  }
  auto mediumThumbnailPath = maybeMediumThumbnailPath->mediumThumbnail;
  if (mediumThumbnailPath.empty()) {
    return;
  }
  if (PB::MediaMap::validImagePath(maybeMediumThumbnailPath->fullPath)) {
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

void TableContentPage::onFinished()
{
  mMediaListItemsCollection.Clear();
  auto rootFolders = mPhotoBook.gallery().foldersList();
  for (auto &path : rootFolders) {
    mMediaListItemsCollection.Append(
        winrt::to_hstring(path.filename().string()));
  }

  MediaListView().ItemsSource(mMediaListItemsCollection);

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
                 (int)(mMediaListItemsCollection.Size() - 1));

  MediaListView().SelectedIndex(mMediaListItemsCollection.Size()-1);
}

void TableContentPage::onFoldersSelectionChanged(
    [[maybe_unused]] ::winrt::Windows::Foundation::IInspectable const &,
    [[maybe_unused]] ::winrt::Microsoft::UI::Xaml::Controls::
        SelectionChangedEventArgs const &)
{
  auto index = MediaListView().SelectedIndex();

  PB::printDebug("Index selected %d\n", index);

  auto &imagesData = PB::Context::inst().data().images();

  auto maybePath = imagesData.groupByIndex(index);

  assert(maybePath.has_value());

  mPhotoBook.gallery().setIterator(imagesData.thumbnailsSet(*maybePath));

  updateGalleryLabel();
}

void TableContentPage::onUnstagedListViewSelectionChanged(
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

  auto galleryIndex = UnstagedListView().SelectedIndex();

  auto navigationSelectedItem =
      winrt::to_string(mMediaListItemsCollection.GetAt(navigationListIndex));

  auto &imagesData = PB::Context::inst().data().images();
  auto  iterator = imagesData.thumbnailsSet(PB::Path(navigationSelectedItem));
  mPhotoBook.gallery().setIterator(iterator);

  if (galleryIndex > -1) {
    mPhotoBook.gallery().setPosition(galleryIndex);
    updateGalleryLabel();
  }
}

void TableContentPage::onStagedListViewSelectionChanged(
    [[maybe_unused]] ::winrt::Windows::Foundation::IInspectable const &,
    [[maybe_unused]] ::winrt::Microsoft::UI::Xaml::Controls::
        SelectionChangedEventArgs const &)
{
  UnstagedListView().DeselectRange(Microsoft::UI::Xaml::Data::ItemIndexRange(
      0, mUnstagingImageCollection.Size()));

  auto stagedImagesIndex = StagedListView().SelectedIndex();
  if (stagedImagesIndex < 0) {
    return;
  }
  auto stagedPhotos = mPhotoBook.stagedPhotos();

  auto &imagesData = PB::Context::inst().data().images();
  auto  iterator = imagesData.stagedIterator();
  iterator.goToPosition(stagedImagesIndex);
  mPhotoBook.gallery().setIterator(iterator);
  updateGalleryLabel();
}

void TableContentPage::onStopped() {}

void TableContentPage::onStarted() {}

void TableContentPage::onPaused() {}

void TableContentPage::onResumed() {}

void TableContentPage::onProgressUpdate(int progress, int reference)
{
  MainProgressBar().Maximum(reference);
  MainProgressBar().Value(progress);
  StatusLabelText().Text(winrt::to_hstring("Status: In progress..."));
}

void TableContentPage::onUnstagedImageAdded(PB::Path fullPath,
                                            PB::Path mediumPath,
                                            PB::Path smallPath, int position)
{
  mUnstagingImageCollection.SetAt(
      position, ImageUIData(winrt::to_hstring(fullPath.string()),
                            winrt::to_hstring(mediumPath.string()),
                            winrt::to_hstring(smallPath.string())));
}

void TableContentPage::onAddingFolder(unsigned size)
{
  for (int i = 0; i < (int)size; ++i) {
    mUnstagingImageCollection.Append(ImageUIData());
  }
}

void TableContentPage::onError(PB::Error error) {}

void TableContentPage::post(std::function<void()> f)
{
  bool success = MainWindow::sMainthreadDispatcher.TryEnqueue(
      DispatcherQueuePriority::Normal, [f{f}]() { f(); });

  assert(success);
}

void TableContentPage::updateGalleryLabel()
{
  auto &gallery = mPhotoBook.gallery();

  auto itemPath = gallery.selectedItem();
  auto rootName = gallery.folderName();

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

void TableContentPage::postponeError(std::string message)
{
  GenericErrorTextBlock().Text(winrt::to_hstring(message));
  post([this]() { genericErrorDialogDisplay(); });
}

void TableContentPage::OnNavigatedTo(
    Microsoft::UI::Xaml::Navigation::NavigationEventArgs e)
{
  std::string fullPath =
      winrt::to_string(winrt::unbox_value<winrt::hstring>(e.Parameter()));

  mPhotoBook.loadProject(PB::Path(fullPath));
}

void TableContentPage::onExportClicked(
    [[maybe_unused]] Windows::Foundation::IInspectable const &,
    [[maybe_unused]] Microsoft::UI::Xaml::RoutedEventArgs const &)
{
  exportDialogDisplay();
}

void TableContentPage::onContentDialogSaveClicked(
    [[maybe_unused]] Windows::Foundation::IInspectable const &,
    [[maybe_unused]] Microsoft::UI::Xaml::Controls::
        ContentDialogButtonClickEventArgs const &)
{
  fireSaveFilePicker(MainWindow::sMainWindowhandle,
                     [this](std::variant<std::string, PB::Error> result) {
                       if (std::holds_alternative<std::string>(result)) {
                         auto newName = std::get<std::string>(result);

                         mPhotoBook.savePhotoBook(newName);
                         Frame().Navigate(
                             winrt::xaml_typename<PhotobookUI::FirstPage>());
                       }
                       else {
                         onError(std::get<PB::Error>(result));
                       }
                     });
}

void TableContentPage::onExportContentDialogClicked(
    [[maybe_unused]] Windows::Foundation::IInspectable const &sender,
    [[maybe_unused]] Microsoft::UI::Xaml::Controls::
        ContentDialogButtonClickEventArgs const &args)
{
  auto        exportName = ExportNameTextBox().Text();
  std::string nativeExportName = winrt::to_string(exportName);

  if (nativeExportName.empty()) {
    postponeError("The given name must not be empty!");
  }
  else if (mStagedImageCollection.Size() == 0) {
    postponeError("There is no staged photo!");
  }
  else {
    fireFolderPicker(MainWindow::sMainWindowhandle, [this, nativeExportName](
                                                        std::string path) {
      std::vector<PB::Path> thumbnailPaths;
      for (auto item : mStagedImageCollection) {
        thumbnailPaths.push_back(winrt::to_string(item.FullPath()));
      }
      mPhotoBook.exportAlbum(nativeExportName, path, thumbnailPaths);
    });
  }
}

void TableContentPage::onContentDialogDiscardClicked(
    [[maybe_unused]] Windows::Foundation::IInspectable const &,
    [[maybe_unused]] Microsoft::UI::Xaml::Controls::
        ContentDialogButtonClickEventArgs const &)
{
  mPhotoBook.discardPhotoBook();
  Frame().Navigate(winrt::xaml_typename<PhotobookUI::FirstPage>());
}

void TableContentPage::onContentDialogCancelClicked(
    [[maybe_unused]] Windows::Foundation::IInspectable const &,
    [[maybe_unused]] Microsoft::UI::Xaml::Controls::
        ContentDialogButtonClickEventArgs const &)
{
}

} // namespace winrt::PhotobookUI::implementation
