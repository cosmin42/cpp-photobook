// Copyright (c) Microsoft Corporation and Contributors.
// Licensed under the MIT License.

// clang-format off
#include "pch.h"
#include "Dashboard.xaml.h"
#if __has_include("Dashboard.g.cpp")
#include "Dashboard.g.cpp"
#endif
// clang-format on

#include "App.xaml.h"
#include "MainWindow.xaml.h"

using namespace winrt;
using namespace Microsoft::UI::Xaml;
using namespace Microsoft::UI::Dispatching;

// To learn more about WinUI, the WinUI project structure,
// and more about our project templates, see: http://aka.ms/winui-project-info.

#include <winrt/Windows.Storage.h>
#include <winrt/Windows.UI.Xaml.Interop.h>

namespace winrt::PhotobookUI::implementation {

PB::Path Dashboard::CurrentAppLocation()
{
  winrt::Windows::Storage::StorageFolder folder =
      winrt::Windows::Storage::ApplicationData::Current().LocalFolder();

  return PB::Path(winrt::to_string(folder.Path()));
}

Microsoft::UI::Xaml::Controls::MenuFlyoutItem Dashboard::RightClickFlyout()
{
  Microsoft::UI::Xaml::Controls::MenuFlyoutItem firstItem;
  firstItem.Text(winrt::to_hstring("Delete"));

  firstItem.Click([this](IInspectable const &obj, RoutedEventArgs const &e) {
    OnDeleteClicked(obj, e);
  });

  return firstItem;
}

Dashboard::Dashboard()
{
  InitializeComponent();

  mAPI = Application::Current()
             .as<winrt::PhotobookUI::implementation::App>()
             ->api();

  mAPI->configure((PB::DashboardListener *)this);

  mProjectsList = winrt::single_threaded_observable_vector<ProjectItem>();
  mMenuFlyout.Items().Append(RightClickFlyout());

  mAPI->recallMetadata();
}

void Dashboard::AddProjectClicked(IInspectable const &, RoutedEventArgs const &)
{
  mAPI->newProject();

  Frame().Navigate(winrt::xaml_typename<TableContentPage>());
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

        mAPI->deleteProject(nativeId);
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
  mAPI->recallProject(winrt::to_string(item.FullPath()));
}

void Dashboard::OnNavigatedTo(
    Microsoft::UI::Xaml::Navigation::NavigationEventArgs args)
{
  if (args.Parameter() != nullptr) {
    winrt::hstring source = unbox_value<winrt::hstring>(args.Parameter());

    if (winrt::to_string(source) == "new-project") {
      mAPI->newProject();

      bool success = MainWindow::sMainThreadDispatcher.TryEnqueue(
          DispatcherQueuePriority::Normal, [this]() {
            Frame().Navigate(winrt::xaml_typename<TableContentPage>());
          });
      PB::basicAssert(success);
    }
  }
}

void Dashboard::onProjectRead()
{
  Frame().Navigate(winrt::xaml_typename<TableContentPage>());
}

void Dashboard::onProjectsMetadataLoaded(
    std::vector<PB::ProjectMetadata> metadata)
{
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

} // namespace winrt::PhotobookUI::implementation
