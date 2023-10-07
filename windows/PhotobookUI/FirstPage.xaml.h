// Copyright (c) Microsoft Corporation and Contributors.
// Licensed under the MIT License.

#pragma once

#include <winrt/Microsoft.UI.Xaml.Controls.Primitives.h>
#include <winrt/Microsoft.UI.Xaml.Input.h>

#include "FirstPage.g.h"
#include "ProjectItem.g.h"

#include <WinrtStorage.h>

#include <pb/Project.h>
#include <pb/persistence/Persistence.h>

using namespace winrt::Windows::Foundation::Collections;

namespace winrt::PhotobookUI::implementation {

struct FirstPage : FirstPageT<FirstPage> {
  FirstPage();

  void AddProjectClicked(Windows::Foundation::IInspectable const    &sender,
                         Microsoft::UI::Xaml::RoutedEventArgs const &args);

  void OnPersistenceDataLoaded();
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

  IObservableVector<ProjectItem> mProjectsList;

  Microsoft::UI::Xaml::Controls::MenuFlyout mMenuFlyout;

  std::optional<int> mLastClickedIndex = std::nullopt;

  PB::Persistence<PB::WinrtStorage> mCentralPersistence;
};
} // namespace winrt::PhotobookUI::implementation

namespace winrt::PhotobookUI::factory_implementation {
struct FirstPage : FirstPageT<FirstPage, implementation::FirstPage> {};
} // namespace winrt::PhotobookUI::factory_implementation
