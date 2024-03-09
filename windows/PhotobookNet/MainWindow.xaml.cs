using Microsoft.UI.Xaml;
using Microsoft.UI.Xaml.Controls;
using Microsoft.UI.Xaml.Controls.Primitives;
using Microsoft.UI.Xaml.Data;
using Microsoft.UI.Xaml.Input;
using Microsoft.UI.Xaml.Media;
using Microsoft.UI.Xaml.Navigation;
using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Runtime.InteropServices.WindowsRuntime;
using Windows.Foundation;
using Windows.Foundation.Collections;
using Windows.Management.Deployment.Preview;
using PhotobookRuntimeComponent;

// To learn more about WinUI, the WinUI project structure,
// and more about our project templates, see: http://aka.ms/winui-project-info.

namespace PhotobookNet
{
    /// <summary>
    /// An empty window that can be used on its own or navigated to within a Frame.
    /// </summary>
    public sealed partial class MainWindow : Window, PhotobookListener
    {
        public MainWindow()
        {
            this.InitializeComponent();
        }

        private async void OnRenameProjectDialogRename(object sender, ContentDialogButtonClickEventArgs args)
        {

        }

        private async void OnRenameProjectDialogCancel(object sender, ContentDialogButtonClickEventArgs args)
        {

        }

        private async void OnListViewRightTapped(object sender, RightTappedRoutedEventArgs args)
        {

        }

        private async void OpenProjectClicked(object sender, ItemClickEventArgs args)
        {

        }

        private async void AddProjectClicked(object sender, RoutedEventArgs args)
        {
        }

        public void OnProjectRead()
        {
            throw new NotImplementedException();
        }

        public void OnProjectRenamed()
        {
            throw new NotImplementedException();
        }

        public void OnMetadataUpdated()
        {
            throw new NotImplementedException();
        }

        public void OnPersistenceError(PBError error)
        {
            throw new NotImplementedException();
        }

        public void OnExportFinished()
        {
            throw new NotImplementedException();
        }

        public void OnError(PBError error)
        {
            throw new NotImplementedException();
        }

        public void OnStagedImageAdded(IList<VirtualImagePtr> photos, int index)
        {
            throw new NotImplementedException();
        }

        public void OnStagedImageRemoved(IList<uint> removedIndexes)
        {
            throw new NotImplementedException();
        }

        public void OnMappingStarted(string path)
        {
            throw new NotImplementedException();
        }

        public void OnMappingFinished(string path)
        {
            throw new NotImplementedException();
        }

        public void OnMappingAborted(string path)
        {
            throw new NotImplementedException();
        }

        public void OnImageUpdated(string root, int row, int index)
        {
            throw new NotImplementedException();
        }

        public void OnProgressUpdate(ProgressInfo definedProgress, ProgressInfo undefinedProgress)
        {
            throw new NotImplementedException();
        }
    }
}
