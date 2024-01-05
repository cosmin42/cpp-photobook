// Copyright (c) Microsoft Corporation and Contributors.
// Licensed under the MIT License.

#pragma once

#include <winrt/Microsoft.UI.Xaml.Controls.Primitives.h>
#include <winrt/Microsoft.UI.Xaml.Input.h>

#include "Dashboard.g.h"
#include "ProjectItem.g.h"

#include <pb/PhotoBook.h>
#include <pb/PhotobookListener.h>
#include <pb/project/Project.h>

using namespace winrt::Windows::Foundation::Collections;

namespace winrt::PhotobookUI::implementation {

struct Dashboard : DashboardT<Dashboard>, public PB::DashboardListener {
  Dashboard();

  void AddProjectClicked(Windows::Foundation::IInspectable const    &sender,
                         Microsoft::UI::Xaml::RoutedEventArgs const &args);

  void OnError(PBDev::Error err);

  void OnListViewRightTapped(
      winrt::Windows::Foundation::IInspectable const &,
      winrt::Microsoft::UI::Xaml::Input::RightTappedRoutedEventArgs const &);

  void OnDeleteClicked(
      [[maybe_unused]] winrt::Windows::Foundation::IInspectable const &,
      [[maybe_unused]] winrt::Microsoft::UI::Xaml::RoutedEventArgs const &);

  void OnRenameClicked(
      [[maybe_unused]] winrt::Windows::Foundation::IInspectable const &,
      [[maybe_unused]] winrt::Microsoft::UI::Xaml::RoutedEventArgs const &);

  void OpenProjectClicked(
      [[maybe_unused]] winrt::Windows::Foundation::IInspectable const &,
      [[maybe_unused]] winrt::Microsoft::UI::Xaml::Controls::
          ItemClickEventArgs const &);

  void OnNavigatedTo(Microsoft::UI::Xaml::Navigation::NavigationEventArgs);

  void onProjectRead() override;

  void
  onProjectsMetadataLoaded(std::vector<PB::ProjectMetadata> metadata) override;

private:
  Microsoft::UI::Xaml::Controls::MenuFlyoutItem DeleteFlyout();
  Microsoft::UI::Xaml::Controls::MenuFlyoutItem RenameFlyout();

  std::string GenerateProjectName();

  IObservableVector<ProjectItem> mProjectsList;

  Microsoft::UI::Xaml::Controls::MenuFlyout mMenuFlyout;

  winrt::hstring mRightClickedId;

  std::shared_ptr<PB::Photobook> mAPI = nullptr;

  std::vector<PB::ProjectMetadata> mMetadata;
};
} // namespace winrt::PhotobookUI::implementation

namespace winrt::PhotobookUI::factory_implementation {
struct Dashboard : DashboardT<Dashboard, implementation::Dashboard> {};
} // namespace winrt::PhotobookUI::factory_implementation
