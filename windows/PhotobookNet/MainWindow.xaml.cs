using Microsoft.UI.Xaml;
using Microsoft.UI.Xaml.Controls;
using System;
using Microsoft.UI.Xaml.Navigation;
using Microsoft.UI;
using Microsoft.UI.Windowing;
using Windows.UI;
using System.Diagnostics;

// To learn more about WinUI, the WinUI project structure,
// and more about our project templates, see: http://aka.ms/winui-project-info.

namespace PhotobookNet
{
    /// <summary>
    /// An empty window that can be used on its own or navigated to within a Frame.
    /// </summary>
    public sealed partial class MainWindow : Window
    {
        private readonly AppWindow mAppWindow = null;

        public MainWindow()
        {
            this.InitializeComponent();

            Frame rootFrame = MainFrame;

            mAppWindow = AppWindow;
            bool success = SetTitleBarColors();

            Debug.Assert(success, "SetTitleBarColors failed");

            var windowHandle = WinRT.Interop.WindowNative.GetWindowHandle(this);

            PhotobookSingletonWrapper.Inst().SetWindowHandle(windowHandle);
            PhotobookSingletonWrapper.Inst().SetChangeTitleHandler((title) => { this.Title = title; });

            rootFrame.NavigationFailed += OnNavigationFailed;

            rootFrame.Navigate(typeof(DashboardPage));
        }

        private void OnNavigationFailed(object sender, NavigationFailedEventArgs e)
        {
            throw new NotImplementedException();
        }

        private bool SetTitleBarColors()
        {
            // Check to see if customization is supported.
            // The method returns true on Windows 10 since Windows App SDK 1.2,
            // and on all versions of Windows App SDK on Windows 11.
            if (AppWindowTitleBar.IsCustomizationSupported())
            {
                AppWindowTitleBar m_TitleBar = mAppWindow.TitleBar;

                // Set active window colors.
                // Note: No effect when app is running on Windows 10
                // because color customization is not supported.
                m_TitleBar.ForegroundColor = Colors.White;
                m_TitleBar.BackgroundColor = Color.FromArgb(0xFF, 0x1A, 0x22, 0x24);
                m_TitleBar.ButtonForegroundColor = Colors.White;
                m_TitleBar.ButtonBackgroundColor = Color.FromArgb(0xFF, 0x1A, 0x22, 0x24);
                m_TitleBar.ButtonHoverForegroundColor = Colors.Gainsboro;
                m_TitleBar.ButtonHoverBackgroundColor = Color.FromArgb(0xFF, 0x1A, 0x22, 0x24);
                m_TitleBar.ButtonPressedForegroundColor = Colors.Gray;
                m_TitleBar.ButtonPressedBackgroundColor = Color.FromArgb(0xFF, 0x1A, 0x22, 0x24);

                // Set inactive window colors.
                // Note: No effect when app is running on Windows 10
                // because color customization is not supported.
                m_TitleBar.InactiveForegroundColor = Colors.Gainsboro;
                m_TitleBar.InactiveBackgroundColor = Color.FromArgb(0xFF, 0x1A, 0x22, 0x24);
                m_TitleBar.ButtonInactiveForegroundColor = Colors.Gainsboro;
                m_TitleBar.ButtonInactiveBackgroundColor = Color.FromArgb(0xFF, 0x1A, 0x22, 0x24);
                return true;
            }
            return false;
        }
    }
}
