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
  mProjectsList = winrt::single_threaded_observable_vector<winrt::hstring>();
  InitializeComponent();
  mPersistence.load([this](std::optional<PB::Error> maybeError) {
    if (maybeError) {
      onError(maybeError.value());
    }
    else {
      onPersistenceDataLoaded();
    }
  });
}

void FirstPage::addProjectClick(IInspectable const &, RoutedEventArgs const &)
{
  Frame().Navigate(winrt::xaml_typename<TableContentPage>());
}

void FirstPage::onPersistenceDataLoaded()
{
  mProjectsList.Clear();
  auto &data = mPersistence.cache();
  for (auto &[key, value] : data) {
    mProjectsList.Append(winrt::to_hstring(key));
    PB::printDebug("%s %s\n", key.c_str(), value.c_str());
  }

  ProjectsListView().ItemsSource(mProjectsList);
}

void FirstPage::onError(PB::Error err)
{
  PB::printError("Error occured on the first page.\n");
}

void FirstPage::OnListViewRightTapped(
    winrt::Windows::Foundation::IInspectable const &,
    winrt::Microsoft::UI::Xaml::Input::RightTappedRoutedEventArgs const &e)
{
}

} // namespace winrt::PhotobookUI::implementation
