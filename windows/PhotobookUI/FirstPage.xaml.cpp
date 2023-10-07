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

#include <winrt/Windows.UI.Xaml.Interop.h>

namespace winrt::PhotobookUI::implementation {

FirstPage::FirstPage()
{
  mProjectsList = winrt::single_threaded_observable_vector<ProjectItem>();
  InitializeComponent();

  Microsoft::UI::Xaml::Controls::MenuFlyoutItem firstItem;
  firstItem.Text(winrt::to_hstring("Delete"));

  firstItem.Click([this](IInspectable const &obj, RoutedEventArgs const &e) {
    OnDeleteClicked(obj, e);
  });

  mMenuFlyout.Items().Append(firstItem);

  mCentralPersistence.load([this](std::optional<PB::Error> maybeError) {
    if (maybeError) {
      OnError(maybeError.value());
    }
    else {
      OnPersistenceDataLoaded();
    }
  });
}

void FirstPage::AddProjectClicked(IInspectable const &, RoutedEventArgs const &)
{
  auto newProject = PB::ProjectsSet<PB::WinrtStorage>().create();

  auto serializedProject =
      std::unordered_map<std::string, std::string>(newProject.details());

  auto [uuidStr, path] = newProject.locationData();

  auto fullPath = path + "\\" + newProject.details().name;

  PB::Persistence<void> newProjectPersistence(fullPath);

  newProjectPersistence.cache().insert(serializedProject.begin(),
                                       serializedProject.end());

  newProjectPersistence.write([](std::optional<PB::Error> maybeError) {
    if (maybeError) {
      PB::printError("Error writing into peristence.\n");
    }
  });

  mCentralPersistence.cache()[uuidStr] = fullPath;
  mCentralPersistence.write([](std::optional<PB::Error> maybeError) {
    if (maybeError) {
      PB::printError("Error writing into peristence.\n");
    }
  });

  auto newPathWin = winrt::to_hstring(fullPath);
  auto boxed = winrt::box_value(newPathWin);

  Frame().Navigate(winrt::xaml_typename<TableContentPage>(), boxed);
}

void FirstPage::OnPersistenceDataLoaded()
{
  mProjectsList.Clear();

  auto &data = mCentralPersistence.cache();
  for (auto &[key, value] : data) {
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

  auto clickedElement = e.OriginalSource()
                            .as<FrameworkElement>()
                            .DataContext()
                            .as<winrt::hstring>();

  auto it = std::find_if(
      mProjectsList.begin(), mProjectsList.end(),
      [clickedElement{clickedElement}](ProjectItem const &projectItem) {
        return clickedElement == projectItem.ItemId();
      });

  if (it != mProjectsList.end()) {
    mLastClickedIndex = (int)(it - mProjectsList.begin());

    PB::printDebug("Index clicked: %d", mLastClickedIndex);

    mMenuFlyout.ShowAt(e.OriginalSource().as<FrameworkElement>());
  }
}

void FirstPage::OnDeleteClicked(
    [[maybe_unused]] winrt::Windows::Foundation::IInspectable const &,
    [[maybe_unused]] winrt::Microsoft::UI::Xaml::RoutedEventArgs const &)
{
  if (mLastClickedIndex) {
    std::string lastClickedKey =
        winrt::to_string(mProjectsList.GetAt(*mLastClickedIndex).ItemId());
    mProjectsList.RemoveAt(*mLastClickedIndex);
    mCentralPersistence.cache().erase(lastClickedKey);
    mCentralPersistence.write([](std::optional<PB::Error>) {});
  }
  mLastClickedIndex = std::nullopt;
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
