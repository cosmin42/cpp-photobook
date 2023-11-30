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

HWND MainWindow::sMainWindowHandle = 0x0;

std::function<void()> MainWindow::sMainExitFunction = nullptr;

winrt::Microsoft::UI::Dispatching::DispatcherQueue
    MainWindow::sMainThreadDispatcher = nullptr;

MainWindow::MainWindow()
{
  InitializeComponent();

  sMainThreadDispatcher =
      winrt::Microsoft::UI::Dispatching::DispatcherQueue::GetForCurrentThread();

  auto windowNative{this->try_as<::IWindowNative>()};
  winrt::check_bool(windowNative);
  windowNative->get_WindowHandle(&sMainWindowHandle);
}

void MainWindow::OnWindowClosed(
    [[maybe_unused]] ::winrt::Windows::Foundation::IInspectable const &,
    [[maybe_unused]] ::winrt::Microsoft::UI::Xaml::WindowEventArgs const &arg)
{
  static bool once = true;
  if (sMainExitFunction && once) {
    sMainExitFunction();
    arg.Handled(true);
    once = false;
  }
}

} // namespace winrt::PhotobookUI::implementation
