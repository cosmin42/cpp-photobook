// Copyright (c) Microsoft Corporation and Contributors.
// Licensed under the MIT License.

#pragma once

#include "FirstPage.g.h"

#include <WinrtStorage.h>

#include <pb/Persistence.h>

using namespace winrt::Windows::Foundation::Collections;

namespace winrt::PhotobookUI::implementation {

struct FirstPage;

class AppPersistence final {
public:
  AppPersistence(FirstPage &parent) : mParent(parent) {}
  ~AppPersistence() = default;

  void onLoaded();

  void onError(PB::Error err);

private:
  FirstPage &mParent;
};

struct FirstPage : FirstPageT<FirstPage> {
  FirstPage();

  void addProjectClick(Windows::Foundation::IInspectable const    &sender,
                       Microsoft::UI::Xaml::RoutedEventArgs const &args);

  void onPersistenceDataLoaded();
  void onError(PB::Error err);

  IObservableVector<winrt::hstring> mProjectsList;

  AppPersistence                    mPersistenceListener;
  PB::Persistence<PB::WinrtStorage> mPersistence;
};
} // namespace winrt::PhotobookUI::implementation

namespace winrt::PhotobookUI::factory_implementation {
struct FirstPage : FirstPageT<FirstPage, implementation::FirstPage> {};
} // namespace winrt::PhotobookUI::factory_implementation
