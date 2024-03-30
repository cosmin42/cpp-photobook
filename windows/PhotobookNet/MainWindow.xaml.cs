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

// To learn more about WinUI, the WinUI project structure,
// and more about our project templates, see: http://aka.ms/winui-project-info.

namespace PhotobookNet
{
    /// <summary>
    /// An empty window that can be used on its own or navigated to within a Frame.
    /// </summary>
    public sealed partial class MainWindow : Window, PhotobookListener
    {

        string mOldProjectName;
        string mRightClickedId;
        MenuFlyout mMenuFlyout;
        readonly ObservableCollection<ProjectItem> mProjectsList;
        string mProjectUUID;

        private PhotobookWin mPhotobook;

        public MainWindow()
        {
            this.InitializeComponent();
            PhotobookSingletonWrapper.Inst().SetThisThreadAsMainDispatcher();
            mPhotobook = PhotobookSingletonWrapper.Inst().Photobook();
            mProjectsList = new ObservableCollection<ProjectItem>();
            mPhotobook.ConfigurePhotobookListener(this);
            mMenuFlyout = new MenuFlyout();
            mMenuFlyout.Items.Add(DeleteFlyout());
            mMenuFlyout.Items.Add(RenameFlyout());

            mPhotobook.RecallMetadata();
        }

        private void OnRenameProjectDialogRename(object sender, ContentDialogButtonClickEventArgs args)
        {
            mPhotobook.GetSettings().Rename(RenameProjectDialogTextBox.Text, mOldProjectName);
        }

        private void OnRenameProjectDialogCancel(object sender, ContentDialogButtonClickEventArgs args)
        {
            // Intentionally empty
        }

        private void OnListViewRightTapped(object sender, RightTappedRoutedEventArgs args)
        {
            var clickedElement = (args.OriginalSource as FrameworkElement).DataContext as ProjectItem;
            mRightClickedId = clickedElement.ItemId;
            mMenuFlyout.ShowAt(args.OriginalSource as FrameworkElement);
        }

        private void OpenProjectClicked(object sender, ItemClickEventArgs args)
        {
            var item = args.ClickedItem as ProjectItem;
            var projectName = item.Name;
            mPhotobook.RecallProject(projectName);
        }

        private void AddProjectClicked(object sender, RoutedEventArgs args)
        {
            var newProjectName = mPhotobook.GenerateProjectName();

            mPhotobook.NewProject(newProjectName);

            dashboardFrame.Navigate(typeof(TableContentPage));
        }

        public void OnProjectRead()
        {
            dashboardFrame.Navigate(typeof(TableContentPage));
        }

        private void OnNavigatedTo(Microsoft.UI.Xaml.Navigation.NavigationEventArgs args)
        {
            mPhotobook.ConfigurePhotobookListener(this);

            if (args.Parameter != null)
            {
                string source = args.Parameter.ToString();
                if (source == "new-project")
                {
                    var newProjectName = mPhotobook.GenerateProjectName();
                    mPhotobook.NewProject(newProjectName);
                    PhotobookSingletonWrapper.Inst().Post(() =>
                    {
                        dashboardFrame.Navigate(typeof(TableContentPage));
                    });
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
            var settings = mPhotobook.GetSettings();

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

        private Microsoft.UI.Xaml.Controls.MenuFlyoutItem DeleteFlyout()
        {
            Microsoft.UI.Xaml.Controls.MenuFlyoutItem deleteItem = new()
            {
                Text = "Delete"
            };
            deleteItem.Click += OnDeleteClicked;

            return deleteItem;
        }

        private void OnDeleteClicked(object sender, RoutedEventArgs e)
        {
            if (!string.IsNullOrEmpty(mRightClickedId))
            {
                for (int i = 0; i < mProjectsList.Count; i++)
                {
                    var id = mProjectsList[i].ItemId;
                    if (id == mRightClickedId)
                    {
                        mProjectsList.RemoveAt(i);
                        mPhotobook.GetSettings().RemoveById(id);
                        break;
                    }
                }
            }
            mRightClickedId = string.Empty;
        }

        private Microsoft.UI.Xaml.Controls.MenuFlyoutItem RenameFlyout()
        {
            Microsoft.UI.Xaml.Controls.MenuFlyoutItem renameItem = new()
            {
                Text = "Rename"
            };

            renameItem.Click += OnRenameClicked;

            return renameItem;
        }

        private void OnRenameClicked(object sender, RoutedEventArgs e)
        {
            if (string.IsNullOrEmpty(mRightClickedId))
            {
                string name = "";

                for (int i = 0; i < mProjectsList.Count; i++)
                {
                    var project = mProjectsList[i];
                    if (project.ItemId == mRightClickedId)
                    {
                        name = Path.GetFileName(project.FullPath);
                        mProjectUUID = project.ItemId;
                        break;
                    }
                }

                System.Diagnostics.Debug.Assert(!string.IsNullOrEmpty(name), "Name is null.");

                mOldProjectName = name;

                RenameProjectDialogTextBox.PlaceholderText = mOldProjectName;
                RenameProjectDialogDisplay();
            }
        }

        private void RenameProjectDialogDisplay()
        {
            PhotobookSingletonWrapper.Inst().Post(async () =>
            {
                await RenameProjectDialog.ShowAsync();
            });

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
