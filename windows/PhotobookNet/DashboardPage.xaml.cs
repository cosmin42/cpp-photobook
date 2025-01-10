using Microsoft.UI.Xaml;
using Microsoft.UI.Xaml.Controls;
using Microsoft.UI.Xaml.Input;
using Microsoft.UI.Xaml.Navigation;
using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.IO;
using System.Linq;
using PhotobookRuntimeComponent;

namespace PhotobookNet
{
    /// <summary>
    /// An empty page that can be used on its own or navigated to within a Frame.
    /// </summary>
    public sealed partial class DashboardPage : Page, PhotobookListener
    {
        // Rename Project
        private string mOldProjectName;

        // The ID of the project that was right-clicked
        private string mRightClickedId;

        // The name of the project that is clicked when opening a project
        private string mLastClickedProjectName;

        // MenuFlyout for right-clicking on a project
        private MenuFlyout mMenuFlyout;

        // The saved projects list
        private ObservableCollection<ProjectItem> mProjectsList = [];

        // Engine reference
        private PhotobookWin mPhotobook;

        static private int SqrtIntF(int size)
        {
            float root = (float)Math.Sqrt(size);
            int intRoot = (int)Math.Ceiling(root);
            return intRoot;
        }

        public DashboardPage()
        {
            this.InitializeComponent();

            mPhotobook = PhotobookSingletonWrapper.Inst().Photobook();

            mPhotobook.ConfigurePhotobookListener(this);

            mMenuFlyout = new MenuFlyout();
            mMenuFlyout.Items.Add(DeleteFlyout());
            mMenuFlyout.Items.Add(RenameFlyout());

            mPhotobook.StartPhotobook();
            mPhotobook.RecallMetadata();
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
            Frame.Navigate(typeof(TableContentPage), mLastClickedProjectName);
        }

        private void AddProjectClicked(object sender, RoutedEventArgs args)
        {
            CreateProjectDialogDisplay();
        }



        private void OnCreateProjectDialogNext(object sender, ContentDialogButtonClickEventArgs args)
        {
            var paperSettings = GetPaperSettings();
            mPhotobook.GetSettings().NewProject(paperSettings);
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
            var comboTypeName = (ComboBoxPaperSettings.SelectedItem as ComboBoxItem).Content.ToString();
            PaperSettings paperSettings = PhotobookWin.GetDefaultSerializedSettings(comboTypeName);

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
            throw new NotImplementedException();
        }

        public void OnProjectRead()
        {
            PhotobookSingletonWrapper.Inst().UpdateTitle(mLastClickedProjectName);
            Frame.Navigate(typeof(TableContentPage));
        }

        protected override void OnNavigatedTo(NavigationEventArgs args)
        {
            mPhotobook.ConfigurePhotobookListener(this);

            System.Diagnostics.Debug.Assert(args.Parameter == null, "No defined parameters");

            PhotobookSingletonWrapper.Inst().UpdateTitle();
        }

        private void OnRenameProjectDialogRename(object sender, ContentDialogButtonClickEventArgs args)
        {
            mPhotobook.GetSettings().Rename(RenameProjectDialogTextBox.Text, mOldProjectName);
        }

        public void OnMetadataUpdated(string focusProjectName)
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

            if (!string.IsNullOrEmpty(focusProjectName))
            {
                PhotobookSingletonWrapper.Inst().UpdateTitle("Photobook Noir - " + focusProjectName);
                Frame.Navigate(typeof(TableContentPage), focusProjectName);
            }
        }

        private MenuFlyoutItem DeleteFlyout()
        {
            MenuFlyoutItem deleteItem = new()
            {
                Text = "Delete"
            };
            deleteItem.Click += OnDeleteClicked;

            return deleteItem;
        }

        private void OnTextBoxPaperWidthBeforeTextChanging(TextBox sender, TextBoxBeforeTextChangingEventArgs args)
        {
            NewProjectDialog.IsPrimaryButtonEnabled = !string.IsNullOrEmpty(args.NewText) && !string.IsNullOrEmpty(TextBoxPaperHeight.Text.ToString()) && !string.IsNullOrEmpty(TextBoxPaperPPI.Text.ToString());

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
            NewProjectDialog.IsPrimaryButtonEnabled = !string.IsNullOrEmpty(TextBoxPaperWidth.Text.ToString()) && !string.IsNullOrEmpty(args.NewText) && !string.IsNullOrEmpty(TextBoxPaperPPI.Text.ToString());

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
            NewProjectDialog.IsPrimaryButtonEnabled = !string.IsNullOrEmpty(TextBoxPaperWidth.Text.ToString()) && !string.IsNullOrEmpty(TextBoxPaperHeight.Text.ToString()) && !string.IsNullOrEmpty(args.NewText);

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

        private MenuFlyoutItem RenameFlyout()
        {
            MenuFlyoutItem renameItem = new()
            {
                Text = "Rename"
            };

            renameItem.Click += OnRenameClicked;

            return renameItem;
        }

        private void OnRenameClicked(object sender, RoutedEventArgs e)
        {
            if (!string.IsNullOrEmpty(mRightClickedId))
            {
                string name = "";

                for (int i = 0; i < mProjectsList.Count; i++)
                {
                    var project = mProjectsList[i];
                    if (project.ItemId == mRightClickedId)
                    {
                        name = Path.GetFileName(project.FullPath);
                        break;
                    }
                }

                System.Diagnostics.Debug.Assert(!string.IsNullOrEmpty(name), "Name is null.");

                mOldProjectName = name.Remove(name.Length - ".photobook".Length);

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

        public void Post(Functor f)
        {
            PhotobookSingletonWrapper.Inst().Post(() =>
            {
                f();
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

        public void OnProgressUpdate(ProgressInfo progress)
        {
            // TODO: show progress here too
        }

        // Deprecated
        public void OnLutAdded(LutIconInfo lutIconInfo)
        {
            PhotobookSingletonWrapper.Inst().lutIconInfos.Add(lutIconInfo);
        }

        public void OnCollageCreated(uint index, VirtualImagePtr newImage)
        {
            throw new NotImplementedException();
        }

        public void OnImageMapped(Guid imageId, VirtualImagePtr image)
        {
            throw new NotImplementedException();
        }
    }
}
