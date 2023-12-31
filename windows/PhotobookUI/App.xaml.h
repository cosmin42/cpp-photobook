// Copyright (c) Microsoft Corporation and Contributors.
// Licensed under the MIT License.

#pragma once

#include "App.xaml.g.h"

#include <pb/PhotoBook.h>

namespace winrt::PhotobookUI::implementation {
struct App : AppT<App> {

  App();

  void OnLaunched(Microsoft::UI::Xaml::LaunchActivatedEventArgs const &);

  void OnNavigationFailed(
      IInspectable const &,
      Microsoft::UI::Xaml::Navigation::NavigationFailedEventArgs const &);

  std::shared_ptr<PB::Photobook> api() const;

private:
  winrt::Microsoft::UI::Xaml::Window mWindow{nullptr};

  std::shared_ptr<PB::Photobook> mAPI;
};
} // namespace winrt::PhotobookUI::implementation
