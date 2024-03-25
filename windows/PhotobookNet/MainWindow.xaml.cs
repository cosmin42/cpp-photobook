using Microsoft.UI.Xaml;
using Microsoft.UI.Xaml.Controls;
using Microsoft.UI.Xaml.Input;
using System;
using System.Collections.Generic;
using PhotobookRuntimeComponent;
using Windows.Foundation.Collections;

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
            PhotobookSingletonWrapper.GetInstance().ConfigurePhotobookListener(this);
            PhotobookSingletonWrapper.GetInstance().RecallMetadata();
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

        private void AddProjectClicked(object sender, RoutedEventArgs args)
        {
            var newProjectName = PhotobookSingletonWrapper.GetInstance().GenerateProjectName();

            PhotobookSingletonWrapper.GetInstance().NewProject(newProjectName);

            dashboardFrame.Navigate(typeof(TableContentPage));
        }

        public void OnProjectRead()
        {
            dashboardFrame.Navigate(typeof(TableContentPage));
        }

        private void OnNavigatedTo(Microsoft.UI.Xaml.Navigation.NavigationEventArgs args)
        {
            PhotobookSingletonWrapper.GetInstance().ConfigurePhotobookListener(this);

            if (args.Parameter != null)
            {
                string source = args.Parameter.ToString();
                if (source == "new-project")
                {
                    var newProjectName = PhotobookSingletonWrapper.GetInstance().GenerateProjectName();
                    PhotobookSingletonWrapper.GetInstance().NewProject(newProjectName);
                    bool success = Microsoft.UI.Dispatching.DispatcherQueue.GetForCurrentThread().TryEnqueue(() =>
                    {
                        dashboardFrame.Navigate(typeof(TableContentPage));
                    });

                    System.Diagnostics.Debug.Assert(success, "Navigation to TableContentPage failed");
                }
            }
        }


        public void OnProjectRenamed()
        {
            for (int i = 0; i < mProjectsList.Count; i++)
            {
                var projectItem = mProjectsList[i];
                if (projectItem.ItemId == mProjectUUID)
                {
                    mProjectsList[i] = new ProjectItem(projectItem.ItemId, projectItem.FullPath, RenameProjectDialogTextBox.Text);
                    break;
                }
            }
        }

        private int SqrtIntF(int size)
        {
            float root = (float)Math.Sqrt(size);
            int intRoot = (int)Math.Floor(root);
            return intRoot;
        }

        public void OnMetadataUpdated()
        {
            var settings = PhotobookSingletonWrapper.GetInstance().GetSettings();

            var projectList = settings.ProjectsList();

            mProjectsList.Clear();
            ProjectsListView.Loaded += (sender, args) =>
            {
                var wrapGrid = (sender as GridView).ItemsPanelRoot as ItemsWrapGrid;

                var squareDimension = SqrtIntF(projectList.Count);

                wrapGrid.MaximumRowsOrColumns = squareDimension;
            };

            foreach (var project in projectList)
            {
                mProjectsList.Add(new ProjectItem(project.ProjectId(), project.ProjectPath(), project.Name()));
            }
            ProjectsListView.ItemsSource = mProjectsList;
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

        IObservableVector<ProjectItem> mProjectsList;

        string mProjectUUID;
    }
}
