// Copyright (c) Microsoft Corporation and Contributors.
// Licensed under the MIT License.

#include "pch.h"

#include "App.xaml.h"
#include "Dashboard.xaml.h"
#include "MainWindow.xaml.h"

#include <PlatformHelper.h>

using namespace winrt;
using namespace Windows::Foundation;
using namespace Microsoft::UI::Xaml;
using namespace Microsoft::UI::Xaml::Controls;
using namespace Microsoft::UI::Xaml::Navigation;
using namespace PhotobookUI;
using namespace PhotobookUI::implementation;
using namespace Windows::Foundation;
using namespace Windows::Storage;

// To learn more about WinUI, the WinUI project structure,
// and more about our project templates, see: http://aka.ms/winui-project-info.

/// <summary>
/// Initializes the singleton application object.  This is the first line of
/// authored code executed, and as such is the logical equivalent of main() or
/// WinMain().
/// </summary>
App::App()
{
  InitializeComponent();

#if defined  _DEBUG &&                                                         \
    !defined DISABLE_XAML_GENERATED_BREAK_ON_UNHANDLED_EXCEPTION
  UnhandledException(
      [this](IInspectable const &, UnhandledExceptionEventArgs const &e) {
        if (IsDebuggerPresent()) {
          auto errorMessage = e.Message();
          __debugbreak();
        }
      });
#endif
}

/// <summary>
/// Invoked when the application is launched.
/// </summary>
/// <param name="e">Details about the launch request and process.</param>
void App::OnLaunched(LaunchActivatedEventArgs const &e)
{
  mWindow = make<MainWindow>();

  mAPI = std::make_shared<PB::Photobook>(PlatformHelper::CurrentAppLocation(), PlatformHelper::CurrentAppInstallationFolder());


  Frame rootFrame = Frame();
  rootFrame.NavigationFailed({this, &App::OnNavigationFailed});
  rootFrame.Navigate(xaml_typename<PhotobookUI::Dashboard>(),
                     box_value(e.Arguments()));

  mWindow.Content(rootFrame);
  mWindow.Activate();
}

void App::OnNavigationFailed(IInspectable const &,
                             NavigationFailedEventArgs const &e)
{
  throw hresult_error(E_FAIL, hstring(L"Failed to load Page ") +
                                  e.SourcePageType().Name);
}

std::shared_ptr<PB::Photobook> App::api() const { return mAPI; }