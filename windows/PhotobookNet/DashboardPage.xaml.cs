using Microsoft.UI.Xaml;
using Microsoft.UI.Xaml.Controls;
using Microsoft.UI.Xaml.Controls.Primitives;
using Microsoft.UI.Xaml.Data;
using Microsoft.UI.Xaml.Input;
using Microsoft.UI.Xaml.Media;
using Microsoft.UI.Xaml.Navigation;
using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.IO;
using System.Linq;
using PhotobookRuntimeComponent;
using Windows.Foundation.Collections;
using WinRT;
using System.Threading.Tasks;
using Microsoft.Graphics.Canvas.Effects;

// To learn more about WinUI, the WinUI project structure,
// and more about our project templates, see: http://aka.ms/winui-project-info.

namespace PhotobookNet
{
    /// <summary>
    /// An empty page that can be used on its own or navigated to within a Frame.
    /// </summary>
    public sealed partial class DashboardPage : Page, PhotobookListener
    {
        string mOldProjectName;
        string mRightClickedId;
        MenuFlyout mMenuFlyout;
        readonly ObservableCollection<ProjectItem> mProjectsList;
        string mProjectUUID;
        string mLastClickedProjectName;

        private PhotobookWin mPhotobook;

        public DashboardPage()
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
            mLastClickedProjectName = projectName;
            mPhotobook.RecallProject(projectName);
        }

        private void AddProjectClicked(object sender, RoutedEventArgs args)
        {
            CreateProjectDialogDisplay();
        }

        private void OnCreateProjectDialogNext(object sender, ContentDialogButtonClickEventArgs args)
        {
            var newProjectName = mPhotobook.GenerateProjectName();
            var paperSettings = GetPaperSettings();
            mPhotobook.NewProject(newProjectName, paperSettings);
            PhotobookSingletonWrapper.Inst().UpdateTitle("Photobook Noir - " + newProjectName);
            Frame.Navigate(typeof(TableContentPage));
        }

        private void UpdateNewProjectDialog(string selectedPaperType)
        {
            var paperSettings = PhotobookWin.GetDefaultSerializedSettings(selectedPaperType);

            TextBoxPaperWidth.BeforeTextChanging -= OnTextBoxPaperWidthBeforeTextChanging;
            TextBoxPaperHeight.BeforeTextChanging -= OnTextBoxPaperHeightBeforeTextChanging;
            TextBoxPaperPPI.BeforeTextChanging -= OnTextBoxPaperPpiBeforeTextChanging;

            TextBoxPaperWidth.Text = paperSettings.Width.ToString();
            TextBoxPaperHeight.Text = paperSettings.Height.ToString();
            TextBoxPaperPPI.Text = paperSettings.Ppi.ToString();

            TextBoxPaperWidth.BeforeTextChanging += OnTextBoxPaperWidthBeforeTextChanging;
            TextBoxPaperHeight.BeforeTextChanging += OnTextBoxPaperHeightBeforeTextChanging;
            TextBoxPaperPPI.BeforeTextChanging += OnTextBoxPaperPpiBeforeTextChanging;
        }

        private PaperSettings GetPaperSettings()
        {
            var paperSettings = PhotobookWin.GetDefaultSerializedSettings((ComboBoxPaperSettings.SelectedItem as ComboBoxItem).Content.ToString());

            paperSettings.Width = int.Parse(TextBoxPaperWidth.Text);
            paperSettings.Height = int.Parse(TextBoxPaperHeight.Text);
            paperSettings.Ppi = int.Parse(TextBoxPaperPPI.Text);

            return paperSettings;
        }

        private void ComboBoxPaperSettingsSelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            var comboBoxItemContent = (ComboBoxPaperSettings.SelectedItem as ComboBoxItem).Content.ToString();
            if (IsPaperSettingsDialogReady() && comboBoxItemContent != "Custom")
            {
                UpdateNewProjectDialog((ComboBoxPaperSettings.SelectedItem as ComboBoxItem).Content.ToString());
            }
        }

        private void OnCreateProjectDialogCancel(object sender, ContentDialogButtonClickEventArgs args)
        {
            // Intentionally empty
        }

        public void OnCollageThumbnailsCreated()
        {

        }

        public void OnProjectRead()
        {
            PhotobookSingletonWrapper.Inst().UpdateTitle(mLastClickedProjectName);
            Frame.Navigate(typeof(TableContentPage));
        }

        protected override void OnNavigatedTo(NavigationEventArgs args)
        {
            mPhotobook.ConfigurePhotobookListener(this);

            if (args.Parameter != null)
            {
                string source = args.Parameter.ToString();
                if (source == "new-project")
                {
                    var newProjectName = mPhotobook.GenerateProjectName();
                    var paperSettings = GetPaperSettings();
                    mPhotobook.NewProject(newProjectName, paperSettings);
                    PhotobookSingletonWrapper.Inst().UpdateTitle(newProjectName);
                    PhotobookSingletonWrapper.Inst().Post(() =>
                    {
                        Frame.Navigate(typeof(TableContentPage));
                    });
                }
                else
                {
                    PhotobookSingletonWrapper.Inst().UpdateTitle();
                }
            }
            else
            {
                PhotobookSingletonWrapper.Inst().UpdateTitle();
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
            int intRoot = (int)Math.Ceiling(root);
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

                var numberOfProjects = projectList.Count;

                var squareDimension = SqrtIntF(numberOfProjects);

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

        private void OnTextBoxPaperWidthBeforeTextChanging(TextBox sender, TextBoxBeforeTextChangingEventArgs args)
        {
            if (!args.NewText.All(char.IsDigit))
            {
                args.Cancel = true;
            }
            else
            {
                ComboBoxPaperSettings.SelectedIndex = ComboBoxPaperSettings.Items.Count - 1;
            }
        }

        private void OnTextBoxPaperHeightBeforeTextChanging(TextBox sender, TextBoxBeforeTextChangingEventArgs args)
        {
            if (!args.NewText.All(char.IsDigit))
            {
                args.Cancel = true;
            }
            else
            {
                ComboBoxPaperSettings.SelectedIndex = ComboBoxPaperSettings.Items.Count - 1;
            }
        }

        private void OnTextBoxPaperPpiBeforeTextChanging(TextBox sender, TextBoxBeforeTextChangingEventArgs args)
        {
            if (!args.NewText.All(char.IsDigit))
            {
                args.Cancel = true;
            }
            else
            {
                ComboBoxPaperSettings.SelectedIndex = ComboBoxPaperSettings.Items.Count - 1;
            }
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

        private bool IsPaperSettingsDialogReady()
        {
            return ComboBoxPaperSettings != null &&
                TextBoxPaperWidth != null &&
                TextBoxPaperHeight != null &&
                TextBoxPaperPPI != null;
        }

        private void CreateProjectDialogDisplay()
        {
            PhotobookSingletonWrapper.Inst().Post(async () =>
            {
                NewProjectDialog.Loaded += (sender, args) =>
                {
                    if (IsPaperSettingsDialogReady())
                    {
                        UpdateNewProjectDialog((ComboBoxPaperSettings.SelectedItem as ComboBoxItem).Content.ToString());
                    }
                };
                await NewProjectDialog.ShowAsync();


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

        public void Post(Functor f)
        {
            PhotobookSingletonWrapper.Inst().Post(() =>
            {
                f();
            });
        }

        public void OnCollageCreated(uint index, VirtualImagePtr newImage)
        {
            throw new NotImplementedException();
        }
    }
}
