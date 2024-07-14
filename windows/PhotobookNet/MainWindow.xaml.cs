using Microsoft.UI.Xaml;
using Microsoft.UI.Xaml.Controls;
using Microsoft.UI.Xaml.Input;
using System;
using System.Collections.Generic;
using PhotobookRuntimeComponent;
using Windows.Foundation.Collections;
using System.Collections.ObjectModel;
using WinRT;
using System.IO;
using System.Threading.Tasks;
using Microsoft.Graphics.Canvas.Effects;
using Microsoft.UI.Xaml.Navigation;

// To learn more about WinUI, the WinUI project structure,
// and more about our project templates, see: http://aka.ms/winui-project-info.

namespace PhotobookNet
{
    /// <summary>
    /// An empty window that can be used on its own or navigated to within a Frame.
    /// </summary>
    public sealed partial class MainWindow : Window
    {
        public MainWindow()
        {
            this.InitializeComponent();

            Frame rootFrame = MainFrame;

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
    }
}
