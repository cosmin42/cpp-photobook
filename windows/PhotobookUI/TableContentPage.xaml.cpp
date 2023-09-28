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
  InitializeComponent();
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

void TableContentPage::onBackClicked(IInspectable const &,
                                     RoutedEventArgs const &)
{
  projectExitDialogDisplay();
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

  auto itemPath = gallery.selectedItem();
  if (!itemPath.has_value()) {
    return;
  }

  int32_t portviewWidth = (int32_t)GalleryCanvas().ActualWidth();

  int32_t portviewHeight = (int32_t)GalleryCanvas().ActualHeight();

  std::shared_ptr<cv::Mat> image = nullptr;
  if (PB::MediaMap::validImagePath(*itemPath)) {
    image = mPhotoBook.loadGalleryImage(itemPath->string(),
                                        {portviewWidth, portviewHeight});
  }
  else {
    image = PB::Process::singleColorImage(portviewWidth, portviewHeight,
                                          {255, 0, 0})();

    image =
        PB::Process::addText({portviewWidth / 2, portviewHeight / 2},
                             itemPath->filename().string(), {0, 255, 0})(image);
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
  mMediaListNative.clear();
  mMediaListItemsCollection.Clear();
  auto rootFolders = mPhotoBook.gallery().foldersList();
  for (auto &path : rootFolders) {
    mMediaListNative.push_back(path.filename().string());
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
}

void TableContentPage::onFoldersSelectionChanged(
    [[maybe_unused]] ::winrt::Windows::Foundation::IInspectable const &,
    [[maybe_unused]] ::winrt::Microsoft::UI::Xaml::Controls::
        SelectionChangedEventArgs const &)
{
  auto index = MediaListView().SelectedIndex();

  PB::printDebug("Index selected %d\n", index);

  mPhotoBook.gallery().selectIndex(index);

  updateGalleryLabel();
}

void TableContentPage::onStopped() {}

void TableContentPage::onStarted() {}

void TableContentPage::onPaused() {}

void TableContentPage::onResumed() {}

void TableContentPage::onProgressUpdate()
{
  StatusLabelText().Text(winrt::to_hstring("Status: In progress..."));
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
    GalleryMainText().Text(winrt::to_hstring(itemPath->filename().string()));
  }
  else if (rootName) {
    GalleryMainText().Text(winrt::to_hstring(rootName->filename().string()));
  }
  else {
    GalleryMainText().Text(winrt::to_hstring("Nothing sleected."));
  }
  GalleryCanvas().Invalidate();
}

void TableContentPage::onAddToTableClicked(
    [[maybe_unused]] Windows::Foundation::IInspectable const    &sender,
    [[maybe_unused]] Microsoft::UI::Xaml::RoutedEventArgs const &e)
{
  auto clickedElement = e.OriginalSource()
                            .as<FrameworkElement>()
                            .DataContext()
                            .as<winrt::hstring>();

  auto it = std::find(mMediaListNative.begin(), mMediaListNative.end(),
                      winrt::to_string(clickedElement));

  if (it != mMediaListNative.end()) {
    int index = (int)(it - mMediaListNative.begin());
    PB::Unused(index);
  }
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
  fireFolderPicker(MainWindow::sMainWindowhandle,
                   [this](std::string path) { mPhotoBook.exportAlbum(path); });
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
