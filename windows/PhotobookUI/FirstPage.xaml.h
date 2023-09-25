// Copyright (c) Microsoft Corporation and Contributors.
// Licensed under the MIT License.

#pragma once

#include <winrt/Microsoft.UI.Xaml.Controls.Primitives.h>
#include <winrt/Microsoft.UI.Xaml.Input.h>

#include "FirstPage.g.h"

#include <WinrtStorage.h>

#include <pb/Persistence.h>
#include <pb/Project.h>

using namespace winrt::Windows::Foundation::Collections;

namespace winrt::PhotobookUI::implementation {

struct FirstPage : FirstPageT<FirstPage> {
  FirstPage();

  void addProjectClick(Windows::Foundation::IInspectable const    &sender,
                       Microsoft::UI::Xaml::RoutedEventArgs const &args);

  void onPersistenceDataLoaded();
  void onError(PB::Error err);

  void OnListViewRightTapped(
      winrt::Windows::Foundation::IInspectable const &,
      winrt::Microsoft::UI::Xaml::Input::RightTappedRoutedEventArgs const &);

  IObservableVector<winrt::hstring> mProjectsList;

  PB::Persistence<PB::WinrtStorage> mPersistence;
  PB::ProjectsSet<PB::WinrtStorage> detectedProjects;
};
} // namespace winrt::PhotobookUI::implementation

namespace winrt::PhotobookUI::factory_implementation {
struct FirstPage : FirstPageT<FirstPage, implementation::FirstPage> {};
} // namespace winrt::PhotobookUI::factory_implementation
