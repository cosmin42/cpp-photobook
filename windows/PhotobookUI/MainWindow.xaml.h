// Copyright (c) Microsoft Corporation and Contributors.
// Licensed under the MIT License.

#pragma once

#include "MainWindow.g.h"

#include <functional>

namespace winrt::PhotobookUI::implementation {
struct MainWindow : MainWindowT<MainWindow> {
  static HWND sMainWindowHandle;

  static winrt::Microsoft::UI::Dispatching::DispatcherQueue
      sMainThreadDispatcher;

  static std::function<void()> sMainExitFunction;

  MainWindow();

  void OnWindowClosed(::winrt::Windows::Foundation::IInspectable const &,
                      ::winrt::Microsoft::UI::Xaml::WindowEventArgs const &);
};
} // namespace winrt::PhotobookUI::implementation

namespace winrt::PhotobookUI::factory_implementation {
struct MainWindow : MainWindowT<MainWindow, implementation::MainWindow> {};
} // namespace winrt::PhotobookUI::factory_implementation
