// Copyright (c) Microsoft Corporation and Contributors.
// Licensed under the MIT License.

// clang-format off
#include "pch.h"
#include "FirstPage.xaml.h"
#if __has_include("FirstPage.g.cpp")
#include "FirstPage.g.cpp"
#endif
// clang-format on

using namespace winrt;
using namespace Microsoft::UI::Xaml;

// To learn more about WinUI, the WinUI project structure,
// and more about our project templates, see: http://aka.ms/winui-project-info.

#include <winrt/Windows.Storage.h>
#include <winrt/Windows.UI.Xaml.Interop.h>

namespace winrt::PhotobookUI::implementation {

PB::Path FirstPage::CurrentAppLocation()
{
  winrt::Windows::Storage::StorageFolder folder =
      winrt::Windows::Storage::ApplicationData::Current().LocalFolder();

  return PB::Path(winrt::to_string(folder.Path()));
}

FirstPage::FirstPage() : mCentralPersistence(CurrentAppLocation())
{
  mProjectsList = winrt::single_threaded_observable_vector<ProjectItem>();
  InitializeComponent();

  Microsoft::UI::Xaml::Controls::MenuFlyoutItem firstItem;
  firstItem.Text(winrt::to_hstring("Delete"));

  firstItem.Click([this](IInspectable const &obj, RoutedEventArgs const &e) {
    OnDeleteClicked(obj, e);
  });

  mMenuFlyout.Items().Append(firstItem);

  mCentralPersistence.read(
      [this](
          std::variant<std::unordered_map<std::string, std::string>, PB::Error>
              mapOrError) {
        if (std::holds_alternative<PB::Error>(mapOrError)) {
          OnError(std::get<PB::Error>(mapOrError));
        }
        else {
          auto &map = std::get<std::unordered_map<std::string, std::string>>(
              mapOrError);
          OnPersistenceDataLoaded(map);
        }
      });
}

void FirstPage::AddProjectClicked(IInspectable const &, RoutedEventArgs const &)
{
  auto newProject = PB::ProjectsSet().create(CurrentAppLocation());

  auto serializedProject =
      std::unordered_map<std::string, std::string>(newProject.details());

  auto [uuidStr, path] = newProject.locationData();

  auto fullPath = path + "\\" + newProject.details().name;

  PB::FilePersistence newProjectPersistence(fullPath);

  newProjectPersistence.write(
      serializedProject, [](std::optional<PB::Error> maybeError) {
        if (maybeError) {
          PB::printError("Error writing into peristence.\n");
        }
      });

  mCentralPersistence.write(
      std::pair<std::string, std::string>(uuidStr, fullPath),
      [](std::optional<PB::Error> maybeError) {
        if (maybeError) {
          PB::printError("Error writing into peristence.\n");
        }
      });

  auto newPathWin = winrt::to_hstring(fullPath);
  auto boxed = winrt::box_value(newPathWin);

  Frame().Navigate(winrt::xaml_typename<TableContentPage>(), boxed);
}

void FirstPage::OnPersistenceDataLoaded(
    std::unordered_map<std::string, std::string> &projects)
{
  mProjectsList.Clear();

  winrt::Microsoft::UI::Xaml::Controls::ItemsWrapGrid wrapGrid =
      ProjectsListView()
          .ItemsPanelRoot()
          .as<winrt::Microsoft::UI::Xaml::Controls::ItemsWrapGrid>();

  auto sqrtIntF = [](int size) {
    float root = sqrt((float)size);
    int   intRoot = (int)floor(root) + 1;
    return intRoot;
  };

  auto squareDimension = sqrtIntF((int)projects.size());
  wrapGrid.MaximumRowsOrColumns(squareDimension);

  for (auto &[key, value] : projects) {
    mProjectsList.Append(
        ProjectItem(winrt::to_hstring(key), winrt::to_hstring(value)));

    PB::printDebug("%s %s\n", key.c_str(), value.c_str());
  }

  ProjectsListView().ItemsSource(mProjectsList);
}

void FirstPage::OnError(PB::Error err)
{
  PB::printError("Error occured on the first page.\n");
}

void FirstPage::OnListViewRightTapped(
    [[maybe_unused]] winrt::Windows::Foundation::IInspectable const &arg,
    [[maybe_unused]] winrt::Microsoft::UI::Xaml::Input::
        RightTappedRoutedEventArgs const &e)
{
  auto clickedElement =
      e.OriginalSource().as<FrameworkElement>().DataContext().as<ProjectItem>();

  mRightClickedId = clickedElement.ItemId();

  mMenuFlyout.ShowAt(e.OriginalSource().as<FrameworkElement>());
}

void FirstPage::OnDeleteClicked(
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

void FirstPage::OpenProjectClicked(
    [[maybe_unused]] winrt::Windows::Foundation::IInspectable const &arg,
    [[maybe_unused]] winrt::Microsoft::UI::Xaml::Controls::
        ItemClickEventArgs const &e)
{
  auto item = e.ClickedItem().as<ProjectItem>();
  Frame().Navigate(winrt::xaml_typename<TableContentPage>(),
                   winrt::box_value(item.FullPath()));
}

} // namespace winrt::PhotobookUI::implementation
