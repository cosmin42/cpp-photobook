// Copyright (c) Microsoft Corporation and Contributors.
// Licensed under the MIT License.

#pragma once

#include <winrt/Microsoft.UI.Xaml.Controls.Primitives.h>
#include <winrt/Microsoft.UI.Xaml.Input.h>

#include "Dashboard.g.h"
#include "ProjectItem.g.h"

#include <pb/Project.h>
#include <pb/persistence/Persistence.h>

using namespace winrt::Windows::Foundation::Collections;

namespace winrt::PhotobookUI::implementation {

struct Dashboard : DashboardT<Dashboard> {

  static PB::Path CurrentAppLocation();

  Dashboard();

  void AddProjectClicked(Windows::Foundation::IInspectable const    &sender,
                         Microsoft::UI::Xaml::RoutedEventArgs const &args);

  std::string CreateProject();

  void OnPersistenceDataLoaded(std::variant<std::vector<PB::ProjectMetadata>,
                                            PB::Error> metadatOrError);
  void OnError(PB::Error err);

  void OnListViewRightTapped(
      winrt::Windows::Foundation::IInspectable const &,
      winrt::Microsoft::UI::Xaml::Input::RightTappedRoutedEventArgs const &);

  void OnDeleteClicked(
      [[maybe_unused]] winrt::Windows::Foundation::IInspectable const &,
      [[maybe_unused]] winrt::Microsoft::UI::Xaml::RoutedEventArgs const &);

  void OpenProjectClicked(
      [[maybe_unused]] winrt::Windows::Foundation::IInspectable const &,
      [[maybe_unused]] winrt::Microsoft::UI::Xaml::Controls::
          ItemClickEventArgs const &);

  void OnNavigatedTo(Microsoft::UI::Xaml::Navigation::NavigationEventArgs);

  IObservableVector<ProjectItem> mProjectsList;

  Microsoft::UI::Xaml::Controls::MenuFlyout mMenuFlyout;

  winrt::hstring mRightClickedId;

  PB::SQLitePersistence mCentralPersistence;
};
} // namespace winrt::PhotobookUI::implementation

namespace winrt::PhotobookUI::factory_implementation {
struct Dashboard : DashboardT<Dashboard, implementation::Dashboard> {};
} // namespace winrt::PhotobookUI::factory_implementation
