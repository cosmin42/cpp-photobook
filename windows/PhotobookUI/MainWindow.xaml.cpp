// Copyright (c) Microsoft Corporation and Contributors.
// Licensed under the MIT License.

// clang-format off
#include "pch.h"
#include "MainWindow.xaml.h"
#if __has_include("MainWindow.g.cpp")
#include "MainWindow.g.cpp"
#endif
// clang-format on

#include <winrt/Windows.ApplicationModel.Core.h>
#include <winrt/Windows.UI.Xaml.Interop.h>

using namespace winrt;
using namespace Microsoft::UI::Xaml;
using namespace Microsoft::UI::Xaml::Controls;
using namespace Windows::UI::Core;

#include <microsoft.ui.xaml.window.h>

// To learn more about WinUI, the WinUI project structure,
// and more about our project templates, see: http://aka.ms/winui-project-info.

namespace winrt::PhotobookUI::implementation {

HWND MainWindow::sMainWindowhandle = 0x0;

winrt::Microsoft::UI::Dispatching::DispatcherQueue
    MainWindow::sMainthreadDispatcher = nullptr;

MainWindow::MainWindow()
{
  InitializeComponent();

  sMainthreadDispatcher =
      winrt::Microsoft::UI::Dispatching::DispatcherQueue::GetForCurrentThread();

  auto windowNative{this->try_as<::IWindowNative>()};
  winrt::check_bool(windowNative);
  windowNative->get_WindowHandle(&sMainWindowhandle);
}

} // namespace winrt::PhotobookUI::implementation
