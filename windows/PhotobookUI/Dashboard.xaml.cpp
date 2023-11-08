// Copyright (c) Microsoft Corporation and Contributors.
// Licensed under the MIT License.

// clang-format off
#include "pch.h"
#include "Dashboard.xaml.h"
#if __has_include("Dashboard.g.cpp")
#include "Dashboard.g.cpp"
#endif
// clang-format on

#include "MainWindow.xaml.h"

using namespace winrt;
using namespace Microsoft::UI::Xaml;
using namespace Microsoft::UI::Dispatching;

// To learn more about WinUI, the WinUI project structure,
// and more about our project templates, see: http://aka.ms/winui-project-info.

#include <winrt/Windows.Storage.h>
#include <winrt/Windows.UI.Xaml.Interop.h>

#include <pb/FilePersistence.h>

namespace winrt::PhotobookUI::implementation {

PB::Path Dashboard::CurrentAppLocation()
{
  winrt::Windows::Storage::StorageFolder folder =
      winrt::Windows::Storage::ApplicationData::Current().LocalFolder();

  return PB::Path(winrt::to_string(folder.Path()));
}

Dashboard::Dashboard() : mCentralPersistence(CurrentAppLocation())
{
  mProjectsList = winrt::single_threaded_observable_vector<ProjectItem>();
  InitializeComponent();

  Microsoft::UI::Xaml::Controls::MenuFlyoutItem firstItem;
  firstItem.Text(winrt::to_hstring("Delete"));

  firstItem.Click([this](IInspectable const &obj, RoutedEventArgs const &e) {
    OnDeleteClicked(obj, e);
  });

  mMenuFlyout.Items().Append(firstItem);

  auto maybeError = mCentralPersistence.connect();
  PB::basicAssert(!maybeError);

  mCentralPersistence.read(
      [this](
          std::variant<std::unordered_map<std::string, std::string>, PB::Error>
              mapOrError) {
        if (std::holds_alternative<PB::Error>(mapOrError)) {
          OnError(std::get<PB::Error>(mapOrError));
        }
        else {
          auto metadatOrError = PB::ProjectMetadata::parse(mapOrError);
          OnPersistenceDataLoaded(metadatOrError);
        }
      });
}

std::string Dashboard::CreateProject()
{
  auto newProject = PB::ProjectsSet().create(CurrentAppLocation());

  auto jsonOrError = PB::Text::serialize<PB::ProjectDetails>(
      0, {"root", newProject.details()});

  PB::basicAssert(std::holds_alternative<PB::Json>(jsonOrError));

  auto uuidStr = boost::uuids::to_string(newProject.details().uuid());

  auto fullPath = newProject.details().projectFile();

  PB::FilePersistence newProjectPersistence(fullPath);

  newProjectPersistence.write(std::get<PB::Json>(jsonOrError).at("root"),
                              [](std::optional<PB::Error> maybeError) {
                                if (maybeError) {
                                  PB::printError(
                                      "Error writing into peristence.\n");
                                }
                              });

  mCentralPersistence.write(
      std::pair<std::string, std::string>(uuidStr, fullPath.string()),
      [](std::optional<PB::Error> maybeError) {
        if (maybeError) {
          PB::printError("Error writing into peristence.\n");
        }
      });

  return fullPath.string();
}

void Dashboard::AddProjectClicked(IInspectable const &, RoutedEventArgs const &)
{
  auto path = CreateProject();

  auto newPathWin = winrt::to_hstring(path);
  auto boxed = winrt::box_value(newPathWin);

  Frame().Navigate(winrt::xaml_typename<TableContentPage>(), boxed);
}

void Dashboard::OnPersistenceDataLoaded(
    std::variant<std::vector<PB::ProjectMetadata>, PB::Error> metadataOrError)
{
  if (std::holds_alternative<PB::Error>(metadataOrError)) {
    return;
  }
  auto &metadata = std::get<std::vector<PB::ProjectMetadata>>(metadataOrError);

  mProjectsList.Clear();

  ProjectsListView().Loaded([size{metadata.size()}](IInspectable const &obj,
                                                    RoutedEventArgs const &) {
    auto sqrtIntF = [](int size) {
      float root = sqrt((float)size);
      int   intRoot = (int)floor(root);
      return intRoot;
    };

    winrt::Microsoft::UI::Xaml::Controls::ItemsWrapGrid wrapGrid =
        obj.as<winrt::Microsoft::UI::Xaml::Controls::GridView>()
            .ItemsPanelRoot()
            .as<winrt::Microsoft::UI::Xaml::Controls::ItemsWrapGrid>();

    auto squareDimension = sqrtIntF((int)size);

    wrapGrid.MaximumRowsOrColumns(squareDimension);
  });

  for (auto &project : metadata) {
    auto [uuid, path] = project.data();
    mProjectsList.Append(
        ProjectItem(winrt::to_hstring(boost::uuids::to_string(uuid)),
                    winrt::to_hstring(path.string())));
  }

  ProjectsListView().ItemsSource(mProjectsList);
}

void Dashboard::OnError(PB::Error err)
{
  PB::printError("Error occured on the first page.\n");
}

void Dashboard::OnListViewRightTapped(
    [[maybe_unused]] winrt::Windows::Foundation::IInspectable const &arg,
    [[maybe_unused]] winrt::Microsoft::UI::Xaml::Input::
        RightTappedRoutedEventArgs const &e)
{
  auto clickedElement =
      e.OriginalSource().as<FrameworkElement>().DataContext().as<ProjectItem>();

  mRightClickedId = clickedElement.ItemId();

  mMenuFlyout.ShowAt(e.OriginalSource().as<FrameworkElement>());
}

void Dashboard::OnDeleteClicked(
    [[maybe_unused]] winrt::Windows::Foundation::IInspectable const &,
    [[maybe_unused]] winrt::Microsoft::UI::Xaml::RoutedEventArgs const &)
{
  if (!mRightClickedId.empty()) {

    for (int i = 0; i < (int)mProjectsList.Size(); ++i) {
      auto id = mProjectsList.GetAt(i).ItemId();
      auto nativeId = winrt::to_string(id);
      if (id == mRightClickedId) {
        mProjectsList.RemoveAt(i);

        mCentralPersistence.deleteEntry(nativeId,
                                        [](std::optional<PB::Error>) {});
        break;
      }
    }
  }
  mRightClickedId = winrt::hstring();
}

void Dashboard::OpenProjectClicked(
    [[maybe_unused]] winrt::Windows::Foundation::IInspectable const &arg,
    [[maybe_unused]] winrt::Microsoft::UI::Xaml::Controls::
        ItemClickEventArgs const &e)
{
  auto item = e.ClickedItem().as<ProjectItem>();
  Frame().Navigate(winrt::xaml_typename<TableContentPage>(),
                   winrt::box_value(item.FullPath()));
}

void Dashboard::OnNavigatedTo(
    Microsoft::UI::Xaml::Navigation::NavigationEventArgs args)
{
  if (args.Parameter() != nullptr) {
    winrt::hstring source = unbox_value<winrt::hstring>(args.Parameter());

    if (winrt::to_string(source) == "new-project") {
      auto path = CreateProject();
      auto newPathWin = winrt::to_hstring(path);
      auto boxed = winrt::box_value(newPathWin);

      bool success = MainWindow::sMainthreadDispatcher.TryEnqueue(
          DispatcherQueuePriority::Normal, [this, boxed{boxed}]() {
            Frame().Navigate(winrt::xaml_typename<TableContentPage>(), boxed);
          });
      PB::basicAssert(success);
    }
  }
}

} // namespace winrt::PhotobookUI::implementation
