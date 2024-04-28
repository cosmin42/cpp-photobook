using Microsoft.UI.Xaml;
using Microsoft.UI.Xaml.Controls;
using Microsoft.UI.Xaml.Input;
using Microsoft.UI.Xaml.Navigation;
using Microsoft.Graphics.Canvas.UI.Xaml;
using PhotobookRuntimeComponent;
using System.Collections.Generic;
using Microsoft.UI.Xaml.Data;
using System.Collections.ObjectModel;
using Windows.Foundation.Collections;
using System.Numerics;
using System;
using System.Threading.Tasks;
using System.Reflection.Metadata;
using System.Collections.Specialized;
using System.Linq;
using WinRT.Interop;
using Microsoft.Graphics.Canvas;
using Windows.AI.MachineLearning;


// To learn more about WinUI, the WinUI project structure,
// and more about our project templates, see: http://aka.ms/winui-project-info.

namespace PhotobookNet
{
    enum DragSource { None, Unstaged, Staged };

    sealed class UISelectionIndex
    {
        public UISelectionIndex()
        {
            ImportListIndex = null;
            UnstagedLineIndex = new List<uint>();
            StagedPhotoIndex = new List<uint>();
        }
        public uint? ImportListIndex { set; get; }
        public List<uint> UnstagedLineIndex { set; get; }
        public List<uint> StagedPhotoIndex { set; get; }
    };

    /// <summary>
    /// The page that displays the content of the photobook.
    /// </summary>
    public sealed partial class TableContentPage : Page, PhotobookListener, StagedImagesListener, ImageMonitorListener
    {

        ObservableCollection<string> mNavigationItemsCollection;
        ObservableCollection<ImageUIData> mUnstagedImageCollection;
        ObservableCollection<ImageUIData> mStagedImageCollection;
        Collection<VirtualImagePtr> mDragAndDropSelectedImages;
        bool mExitFlag = false;
        bool mNewProjectFlag = false;
        HashSet<string> mLoadedFinishedImportFolders;
        DragSource mDragSource = DragSource.None;
        bool mLinesExclusiveSelection = false;
        bool mBackFlag = false;

        private PhotobookWin mPhotobook;

        private static (int, int) ScreenSize()
        {
            // TODO: Do the actual retrieval of the screen size
            int width = 2560;
            int height = 1440;
            return (width, height);
        }

        public TableContentPage()
        {
            this.InitializeComponent();
            mPhotobook = PhotobookSingletonWrapper.Inst().Photobook();
            mPhotobook.ConfigurePhotobookListener(this);
            Int32Pair screenSize = new Int32Pair(ScreenSize().Item1, ScreenSize().Item2);
            mPhotobook.ConfigureScreenSize(screenSize);

            mPhotobook.ConfigureStagedImagesListener(this);
            mPhotobook.ConfigureImageMonitorListener(this);

            mNavigationItemsCollection = new ObservableCollection<string>();
            mUnstagedImageCollection = new ObservableCollection<ImageUIData>();
            mStagedImageCollection = new ObservableCollection<ImageUIData>();

            PhotobookSingletonWrapper.Inst().SetOnWindowClosed(() =>
            {
                var isSaved = mPhotobook.GetSettings().IsSaved(mPhotobook.GetImageViews().ImageMonitor().Unstaged(),
                mPhotobook.GetImageViews().StagedImages().StagedPhotos(),
                mPhotobook.GetImageViews().ImageMonitor().RowList());

                if (isSaved)
                {
                    PhotobookSingletonWrapper.Inst().Post(() =>
                    {
                        Application.Current.Exit();
                    });
                }
                else
                {
                    mExitFlag = true;
                    PhotobookSingletonWrapper.Inst().Post(async () =>
                    {
                        await SaveProjectDialog.ShowAsync();
                    });
                }
            });

            UnstagedListView.ItemsSource = mUnstagedImageCollection;
            StagedListView.ItemsSource = mStagedImageCollection;

            KeyUp += (object sender, Microsoft.UI.Xaml.Input.KeyRoutedEventArgs args) =>
            {
                OnKeyPressed(sender, args);
            };

            mStagedImageCollection.CollectionChanged += (object sender, NotifyCollectionChangedEventArgs args) =>
            {
                if (args.Action == NotifyCollectionChangedAction.Add)
                {
                    System.Collections.IList newItems = args.NewItems;
                    for (int i = 0; i < newItems.Count; i++)
                    {
                        var index = mStagedImageCollection.IndexOf(newItems[i] as ImageUIData);
                        OnStagedImageCollectionChanged(sender as IObservableVector<ImageUIData>, CollectionChange.ItemInserted, index);
                    }
                }
                else if (args.Action == NotifyCollectionChangedAction.Remove)
                {
                    var startingIndex = args.NewStartingIndex;
                    var countOfRemovedItems = args.NewItems.Count;
                    for (int i = 0; i < args.NewItems.Count; i++)
                    {
                        OnStagedImageCollectionChanged(sender as IObservableVector<ImageUIData>, CollectionChange.ItemRemoved, startingIndex + i);
                    }
                }
                else
                {
                    System.Diagnostics.Debug.Assert(false, "Invalid change type");
                }
            };

            mUnstagedImageCollection.CollectionChanged += (object sender, NotifyCollectionChangedEventArgs args) =>
            {
                if ((sender as ObservableCollection<ImageUIData>).Count == 0)
                {
                    AddMediaButton.VerticalAlignment = VerticalAlignment.Center;
                    RemoveMediaButton.Visibility = Visibility.Collapsed;
                }
                else
                {
                    AddMediaButton.VerticalAlignment = VerticalAlignment.Bottom;
                    RemoveMediaButton.VerticalAlignment = VerticalAlignment.Bottom;
                    RemoveMediaButton.Visibility = Visibility.Visible;
                }
            };
        }

        private void MUnstagedImageCollection_CollectionChanged(object sender, NotifyCollectionChangedEventArgs e)
        {
            throw new NotImplementedException();
        }

        private int CanvasMinWidth
        {
            get
            {
                var paperSettings = mPhotobook.GetSettings().GetPaperSettings();
                System.Diagnostics.Debug.Assert(paperSettings.Ppi() > 0, "Width is 0");
                double ratio = PaperToCanvasRatio(paperSettings.Width(), paperSettings.Height(), 438, 310);

                if (ratio > 1)
                {
                    return (int)Math.Floor(paperSettings.Width() / ratio);
                }
                else
                {
                    return paperSettings.Width();
                }
            }
            set { }
        }

        private int CanvasMinHeight
        {
            get
            {
                var paperSettings = mPhotobook.GetSettings().GetPaperSettings();
                System.Diagnostics.Debug.Assert(paperSettings.Ppi() > 0, "Height is 0");
                double ratio = PaperToCanvasRatio(paperSettings.Width(), paperSettings.Height(), 438, 310);

                if (ratio > 1)
                {
                    return (int)Math.Floor(paperSettings.Height() / ratio);
                }
                else
                {
                    return paperSettings.Height();
                }
            }
            set { }
        }

        private void LoadImages()
        {
            var rowList = mPhotobook.GetImageViews().ImageMonitor().RowList();

            for (int i = 0; i < rowList.Count; i++)
            {
                mNavigationItemsCollection.Add(System.IO.Path.GetFileName(rowList[i]));
            }

            MediaListView.ItemsSource = mNavigationItemsCollection;

            if (rowList.Count > 0)
            {
                MediaListView.SelectedIndex = mNavigationItemsCollection.Count - 1;

                var lastROwIndex = rowList.Count - 1;
                var rootPath = mPhotobook.GetImageViews().ImageMonitor().RowPath((uint)lastROwIndex);

                for (int i = 0; i < mPhotobook.GetImageViews().ImageMonitor().RowSize((uint)lastROwIndex); i++)
                {
                    var image = mPhotobook.GetImageViews().ImageMonitor().Image((uint)lastROwIndex, (uint)i);
                    mUnstagedImageCollection.Insert(i, new ImageUIData(image.keyPath(),
                        image.frontend().fullPath(),
                        image.frontend().mediumPath(),
                        image.frontend().smallPath(),
                        image.processed()));
                }
            }

            var stagedPictures = mPhotobook.GetImageViews().StagedImages().StagedPhotos();

            for (int i = 0; i < stagedPictures.Count; ++i)
            {
                mStagedImageCollection.Append(new ImageUIData(stagedPictures[i].keyPath(),
                                       stagedPictures[i].frontend().fullPath(),
                                       stagedPictures[i].frontend().mediumPath(),
                                       stagedPictures[i].frontend().smallPath(),
                                       stagedPictures[i].processed()));
            }
        }

        private void PostponeError(string message)
        {
            GenericErrorTextBlock.Text = message;
            PhotobookSingletonWrapper.Inst().Post(async () =>
            {
                await GenericErrorDialog.ShowAsync();
            });
        }

        /* Menu Bar */
        private void OnExportClicked(object sender, RoutedEventArgs args)
        {
            if (mStagedImageCollection.Count == 0)
            {
                PostponeError("There is no staged photo!");
            }
            else
            {
                PhotobookSingletonWrapper.Inst().Post(async () => { await ExportContentDialog.ShowAsync(); });
            }
        }

        private void OnBackClicked(object sender, RoutedEventArgs args)
        {
            var isSaved = mPhotobook.GetSettings().IsSaved(mPhotobook.GetImageViews().ImageMonitor().Unstaged(),
                mPhotobook.GetImageViews().StagedImages().StagedPhotos(),
                mPhotobook.GetImageViews().ImageMonitor().RowList());
            if (!isSaved)
            {
                mBackFlag = true;
                PhotobookSingletonWrapper.Inst().Post(async () => { await SaveProjectDialog.ShowAsync(); });
            }
            else
            {
                mPhotobook.UnloadProject();
                Frame.Navigate(typeof(DashboardPage));
            }
        }

        private void OnAboutClicked(object sender, RoutedEventArgs args)
        {
            GenericMessageTextBlock.Text = "Photobook v1.0\n\nDeveloped by:\n\n- A\n- B\n- C\n- D\n- E\n- F\n- G\n- H\n- I\n- J\n- K\n- L\n- M\n- N\n- O\n- P\n- Q\n- R\n- S\n- T\n- U\n- V\n- W\n- X\n- Y\n- Z";
            PhotobookSingletonWrapper.Inst().Post(async () =>
            {
                await GnericMessage.ShowAsync();
            });
        }

        private void OnLicenseClicked(object sender, RoutedEventArgs args)
        {
            // TODO: Use a resx file
        }

        private void OnTipsClicked(object sender, RoutedEventArgs args)
        {
            //ILE
        }

        private void OnExitClicked(object sender, RoutedEventArgs args)
        {
            PhotobookSingletonWrapper.Inst().Post(() =>
            {
                Application.Current.Exit();
            });
            mExitFlag = true;
        }

        private void OnSaveClicked(object sender, RoutedEventArgs args)
        {
            mPhotobook.GetSettings().Save(mPhotobook.GetSettings().CurrentProjectUUID(),
                mPhotobook.GetImageViews().ImageMonitor().Unstaged(),
                mPhotobook.GetImageViews().StagedImages().StagedPhotos(),
                mPhotobook.GetImageViews().ImageMonitor().RowList());
        }

        private void OnSaveAsClicked(object sender, RoutedEventArgs args)
        {
            PhotobookSingletonWrapper.Inst().Post(async () => { await RenameProjectDialog.ShowAsync(); });
        }

        private void OnNewClicked(object sender, RoutedEventArgs args)
        {
            var isSaved = mPhotobook.GetSettings().IsSaved(mPhotobook.GetImageViews().ImageMonitor().Unstaged(),
                mPhotobook.GetImageViews().StagedImages().StagedPhotos(),
                mPhotobook.GetImageViews().ImageMonitor().RowList());

            if (isSaved)
            {
                mPhotobook.UnloadProject();
                Frame.Navigate(typeof(DashboardPage), "new-project");
            }
            else
            {
                mNewProjectFlag = true;
                PhotobookSingletonWrapper.Inst().Post(async () => { await SaveProjectDialog.ShowAsync(); });
            }
        }

        private void Right()
        {
            var selection = GetSelectionIndex();

            int nextIndex = 0;

            if (selection.UnstagedLineIndex.Count != 0)
            {
                System.Diagnostics.Debug.Assert(selection.UnstagedLineIndex.Count == 1, "UnstagedLineIndex is not 1");
                nextIndex = (int)(selection.UnstagedLineIndex[0] + 1) % (int)mUnstagedImageCollection.Count;

                UnstagedListView.DeselectRange(new ItemIndexRange(0, (uint)mUnstagedImageCollection.Count));
                UnstagedListView.SelectRange(new ItemIndexRange((int)nextIndex, 1));
            }
            else if (selection.StagedPhotoIndex.Count != 0)
            {
                System.Diagnostics.Debug.Assert(selection.StagedPhotoIndex.Count == 1, "StagedPhotoIndex is not 1");
                nextIndex = (int)(selection.StagedPhotoIndex[0] + 1) % (int)mStagedImageCollection.Count;

                StagedListView.DeselectRange(new ItemIndexRange(0, (uint)mStagedImageCollection.Count));
                StagedListView.SelectRange(new ItemIndexRange((int)nextIndex, 1));
            }
        }

        private void Left()
        {
            var selection = GetSelectionIndex();

            int nextIndex = 0;

            if (selection.UnstagedLineIndex.Count != 0)
            {
                System.Diagnostics.Debug.Assert(selection.UnstagedLineIndex.Count == 1, "UnstagedLineIndex is not 1");
                if (selection.UnstagedLineIndex[0] == 0)
                {
                    nextIndex = (int)mUnstagedImageCollection.Count - 1;
                }
                else
                {
                    nextIndex = (int)(selection.UnstagedLineIndex[0] - 1);
                }

                UnstagedListView.DeselectRange(new ItemIndexRange(0, (uint)mUnstagedImageCollection.Count));
                UnstagedListView.SelectRange(new ItemIndexRange((int)nextIndex, 1));
            }
            else if (selection.StagedPhotoIndex.Count != 0)
            {
                System.Diagnostics.Debug.Assert(selection.StagedPhotoIndex.Count == 1, "StagedPhotoIndex is not 1");
                if (selection.StagedPhotoIndex[0] == 0)
                {
                    nextIndex = (int)mStagedImageCollection.Count - 1;
                }
                else
                {
                    nextIndex = (int)(selection.StagedPhotoIndex[0] - 1);
                }

                StagedListView.DeselectRange(new ItemIndexRange(0, (uint)mStagedImageCollection.Count));
                StagedListView.SelectRange(new ItemIndexRange((int)nextIndex, 1));
            }
        }

        private void UpdateGalleryLabel()
        {
            var selection = GetSelectionIndex();

            VirtualImagePtr imagePtr = null;

            if (selection.StagedPhotoIndex.Count != 0)
            {
                imagePtr = mPhotobook.GetImageViews().StagedImages().Picture((int)selection.StagedPhotoIndex[0]);
            }
            else if (selection.UnstagedLineIndex.Count != 0)
            {
                imagePtr = mPhotobook.GetImageViews().ImageMonitor().Image((uint)selection.ImportListIndex.Value, (uint)selection.UnstagedLineIndex[0]);
            }
            else
            {
                GalleryLeftButton.IsEnabled = false;
                GalleryRightButton.IsEnabled = false;
                GalleryMainText.Text = "No photo selected";
                return;
            }

            GalleryMainText.Text = System.IO.Path.GetFileName(imagePtr.keyPath());

            GalleryLeftButton.IsEnabled = imagePtr != null;
            GalleryRightButton.IsEnabled = imagePtr != null;
        }

        private void OnUndoClicked(object sender, RoutedEventArgs args) { }

        private void OnRedoClicked(object sender, RoutedEventArgs args) { }

        private void OnPreferencesClicked(object sender, RoutedEventArgs args) { }

        /* Navigation */

        private async Task FireFolderPicker(Action<string> onSuccess)
        {
            Windows.Storage.Pickers.FolderPicker folderPicker = new Windows.Storage.Pickers.FolderPicker();
            WinRT.Interop.InitializeWithWindow.Initialize(folderPicker, PhotobookSingletonWrapper.Inst().GetWindowHandle());
            folderPicker.FileTypeFilter.Add("*");
            var folder = await folderPicker.PickSingleFolderAsync();
            if (folder != null)
            {
                onSuccess(folder.Path);
            }
        }

        private async void OnImportFolderAdded(object sender, RoutedEventArgs args)
        {
            await FireFolderPicker(onSuccess: (path) =>
            {
                PhotobookSingletonWrapper.Inst().Post(() =>
                {
                    mPhotobook.AddImportFolder(path);
                });
            });
        }

        private void OnImportFolderRemoved(object sender, RoutedEventArgs args)
        {
            var selection = MediaListView.SelectedIndex;
            System.Diagnostics.Debug.Assert(selection >= 0, "Selection is less than 0");
            var rowPath = mPhotobook.GetImageViews().ImageMonitor().RowPath((uint)selection);
            mPhotobook.RemoveImportFolder(rowPath);
        }

        private void OnImportSelectionChanged(object sender, SelectionChangedEventArgs args)
        {
            UpdateUnstagedPhotoLine();
            if (UnstagedListView.Items.Count > 0)
            {
                UnstagedListView.SelectRange(new ItemIndexRange(0, 1));
            }
            else if (StagedListView.Items.Count > 0)
            {
                StagedListView.SelectRange(new ItemIndexRange(0, 1));
            }
        }

        void OnStagedImageCollectionChanged(IObservableVector<ImageUIData> sender, Windows.Foundation.Collections.CollectionChange changeType, int changedIndex)
        {
            if (mDragSource == DragSource.Staged)
            {
                return;
            }

            if (changeType == Windows.Foundation.Collections.CollectionChange.ItemInserted)
            {
                mPhotobook.GetImageViews().StagedImages().PopImages((int)changedIndex);
            }
            else if (changeType == Windows.Foundation.Collections.CollectionChange.ItemRemoved)
            {
                mPhotobook.GetImageViews().StagedImages().StashImages(new List<uint> { (uint)changedIndex });
            }
            else
            {
                System.Diagnostics.Debug.Assert(false, "Invalid change type");
            }
        }

        /* Gallery */
        private void OnGalleryLeft(object sender, RoutedEventArgs args)
        {
            Left();
            UpdateGalleryLabel();
        }

        private void OnGalleryRight(object sender, RoutedEventArgs args)
        {
            Right();
            UpdateGalleryLabel();
        }

        private void OnCanvasDraw(CanvasControl sender, CanvasDrawEventArgs args)
        {
            var selection = GetSelectionIndex();
            var session = args.DrawingSession;
            if (selection.StagedPhotoIndex.Count == 0 && selection.UnstagedLineIndex.Count == 0)
            {
                session.Clear(Microsoft.UI.Colors.Black);
                return;
            }

            int portviewWidth = (int)GalleryCanvas.ActualWidth;
            int portviewHeight = (int)GalleryCanvas.ActualHeight;
            // TODO Add managed implementation.

            VirtualImagePtr imagePtr = null;

            if (selection.UnstagedLineIndex.Count != 0)
            {
                imagePtr = mPhotobook.GetImageViews().ImageMonitor().Image(selection.ImportListIndex.Value, selection.UnstagedLineIndex[0]);
            }
            else if (selection.StagedPhotoIndex.Count != 0)
            {
                imagePtr = mPhotobook.GetImageViews().StagedImages().Picture((int)selection.StagedPhotoIndex[0]);
            }

            System.Diagnostics.Debug.Assert(imagePtr != null);

            // TODO: make this static as it is always used.
            byte[] byteArray = new byte[portviewWidth * portviewHeight * 4];

            imagePtr.GalleryProjection(byteArray, portviewWidth, portviewHeight);

            CanvasDevice device = CanvasDevice.GetSharedDevice();

            CanvasBitmap bitmap = Microsoft.Graphics.Canvas.CanvasBitmap.CreateFromBytes(device, byteArray, portviewWidth, portviewHeight, Windows.Graphics.DirectX.DirectXPixelFormat.B8G8R8X8UIntNormalized);

            session.DrawImage(bitmap);
        }

        /* Book Lines */

        protected override void OnNavigatedTo(NavigationEventArgs args)
        {
            LoadImages();
            if (UnstagedListView.Items.Count > 0)
            {
                UnstagedListView.SelectRange(new ItemIndexRange(0, 1));
            }
            else if (StagedListView.Items.Count > 0)
            {
                StagedListView.SelectRange(new ItemIndexRange(0, 1));
            }
        }

        private void OnUnstagedPhotosSelectionChanged(object sender, SelectionChangedEventArgs args)
        {
            if (!mLinesExclusiveSelection)
            {
                mLinesExclusiveSelection = true;
                StagedListView.DeselectRange(new ItemIndexRange(0, (uint)mStagedImageCollection.Count));
                UpdateGalleryLabel();
                GalleryCanvas.Invalidate();
            }
            mLinesExclusiveSelection = false;
        }

        private void OnStagedPhotosSelectionChanged(object sender, SelectionChangedEventArgs args)
        {
            if (!mLinesExclusiveSelection)
            {
                mLinesExclusiveSelection = true;
                UnstagedListView.DeselectRange(new ItemIndexRange(
                    0, (uint)mUnstagedImageCollection.Count));

                UpdateGalleryLabel();
                GalleryCanvas.Invalidate();
            }
            mLinesExclusiveSelection = false;
        }

        void UpdateStatusBar(ProgressInfo defined, ProgressInfo undefined)
        {
            if (defined.jobsProgress().Count == 0 && undefined.jobsProgress().Count == 0)
            {
                StatusLabelText.Text = "Idle";
            }
            else
            {
                string concatened = "";

                foreach (var name in defined.jobsProgress())
                {
                    concatened = defined.jobType().ToString() + " " + concatened + name + ", ";
                }

                foreach (var name in undefined.jobsProgress())
                {
                    concatened = undefined.jobType().ToString() + " " + concatened + name + ", " + "(" + undefined.progress().ToString() + ")";
                }

                StatusLabelText.Text = concatened;
            }
        }

        private void OnStagedDragItemsStarting(object sender, DragItemsStartingEventArgs args)
        {
            mDragSource = DragSource.Staged;
        }

        private void OnStagedDragItemsCompleted(object sender, DragItemsCompletedEventArgs args)
        {
            if (mDragSource == DragSource.Staged)
            {
                mDragSource = DragSource.None;
            }
        }

        private void OnUnstagedPhotosDragStarted(object sender, DragItemsStartingEventArgs args)
        {
            bool allowDrag = true;
            foreach (var item in args.Items)
            {
                var image = item as ImageUIData;
                if (!image.Processed)
                {
                    allowDrag = false;
                    break;
                }
                var keyPath = image.KeyPath;
                var imagePtr = mPhotobook.GetImageViews().ImageMonitor().Image(keyPath);
                mDragAndDropSelectedImages.Add(imagePtr);
            }
            if (allowDrag)
            {
                mDragSource = DragSource.Unstaged;
            }
            else
            {
                mDragSource = DragSource.None;
            }
        }

        private void OnDropIntoStagedPhotos(object sender, DragEventArgs args)
        {
            if (mDragSource == DragSource.None)
            {
                return;
            }

            // WORKAROUND BEGIN

            var dropRelativeToStageListViewPosition = args.GetPosition(sender as ListView);

            if (dropRelativeToStageListViewPosition.Y < 0 || dropRelativeToStageListViewPosition.Y > StagedListView.ActualHeight)
            {
                return;
            }

            int insertPosition = StagedListView.Items.Count;

            foreach (var item in StagedListView.Items)
            {
                var itemContainer = StagedListView.ContainerFromItem(item) as ListViewItem;

                var itemIndex = StagedListView.IndexFromContainer(itemContainer);

                var dropRelativeToItem = args.GetPosition(itemContainer);

                if (dropRelativeToItem.X < itemContainer.ActualWidth)
                {
                    insertPosition = itemIndex;
                    break;
                }
            }

            // WORKAROUND END

            List<VirtualImagePtr> copyOfDraggedImages = new List<VirtualImagePtr>();

            foreach (var image in mDragAndDropSelectedImages)
            {
                copyOfDraggedImages.Add(PhotobookRuntimeComponent.PhotobookWin.copyImage(image));
            }

            OnStagedImageAdded(copyOfDraggedImages, insertPosition);

            mDragAndDropSelectedImages.Clear();
        }

        private void OnDragOverStagedPhotos(object sender, DragEventArgs args)
        {
            args.AcceptedOperation = Windows.ApplicationModel.DataTransfer.DataPackageOperation.Copy;
        }

        private void OnClickedOutsideList(object sender, TappedRoutedEventArgs args)
        {
            // Intentionally left blanc
        }

        private void UnstagedSelectAllInvoked(KeyboardAccelerator sender, KeyboardAcceleratorInvokedEventArgs args)
        {
            UnstagedListView.SelectAll();
        }

        /* Dialogs - Save */
        private void OnContentDialogDiscardClicked(object sender, ContentDialogButtonClickEventArgs args)
        {
            mPhotobook.UnloadProject();
            if (mExitFlag)
            {
                PhotobookSingletonWrapper.Inst().Post(() => { Microsoft.UI.Xaml.Application.Current.Exit(); });
            }
            else
            {
                Frame.Navigate(typeof(DashboardPage));
            }
        }

        private void OnContentDialogCancelClicked(object sender, ContentDialogButtonClickEventArgs args)
        {
            // ILB
        }

        private void OnRenameProjectDialogRename(object sender, ContentDialogButtonClickEventArgs args)
        {
            mPhotobook.GetSettings().Rename(RenameProjectDialogTextBox.Text, "");
        }

        private void OnRenameProjectDialogCancel(object sender, ContentDialogButtonClickEventArgs args)
        {
            if (mExitFlag)
            {
                PhotobookSingletonWrapper.Inst().Post(() => { Microsoft.UI.Xaml.Application.Current.Exit(); });
            }
        }

        private void OnSaveProject(object sender, ContentDialogButtonClickEventArgs args)
        {
            mPhotobook.GetSettings().Save(mPhotobook.GetSettings().CurrentProjectUUID(),
                mPhotobook.GetImageViews().ImageMonitor().Unstaged(),
                mPhotobook.GetImageViews().StagedImages().StagedPhotos(),
                mPhotobook.GetImageViews().ImageMonitor().RowList());
            if (mExitFlag)
            {
                PhotobookSingletonWrapper.Inst().Post(() => { Microsoft.UI.Xaml.Application.Current.Exit(); });
            }
            if (mNewProjectFlag)
            {
                mNewProjectFlag = false;
                mPhotobook.UnloadProject();
                Frame.Navigate(typeof(DashboardPage), "new-project");
            }
            if (mBackFlag)
            {
                mBackFlag = false;
                mPhotobook.UnloadProject();
                Frame.Navigate(typeof(DashboardPage));
            }
        }


        private void OnCancelSavingProject(object sender, ContentDialogButtonClickEventArgs args)
        {
            if (mExitFlag)
            {
                PhotobookSingletonWrapper.Inst().Post(() => { Microsoft.UI.Xaml.Application.Current.Exit(); });
            }
            if (mNewProjectFlag)
            {
                mNewProjectFlag = false;
                mPhotobook.UnloadProject();
                Frame.Navigate(typeof(DashboardPage), "new-project");
            }
            if (mBackFlag)
            {
                mBackFlag = false;
                mPhotobook.UnloadProject();
                Frame.Navigate(typeof(DashboardPage));
            }
        }

        /* Dialogs - Export */
        private async void OnExportContentDialogClicked(object sender, ContentDialogButtonClickEventArgs args)
        {
            var exportName = ExportNameTextBox.Text;
            List<ExportType> exportSelection = new();
            if (OptionPDFCheckBox.IsChecked == true)
            {
                exportSelection.Add(ExportType.Pdf);
            }
            if (OptionJPGCheckBox.IsChecked == true)
            {
                exportSelection.Add(ExportType.Jpg);
            }
            if (OptionPDFLibharuCheckBox.IsChecked == true)
            {
                exportSelection.Add(ExportType.PdfLibharu);
            }
            if (exportSelection.Count == 0)
            {
                PostponeError("Check a format!");
                return;
            }

            if (string.IsNullOrEmpty(exportName))
            {
                PostponeError("The given name must not be empty!");
                return;
            }
            else
            {
                if (exportSelection.Count > 0)
                {
                    await FireFolderPicker(onSuccess: (path) =>
                    {
                        if (exportSelection.Contains(ExportType.Pdf))
                        {
                            mPhotobook.ExportPDFAlbum(path, exportName);
                        }
                        if (exportSelection.Contains(ExportType.Jpg))
                        {
                            mPhotobook.ExportJPGAlbum(path, exportName);
                        }
                        if (exportSelection.Contains(ExportType.PdfLibharu))
                        {
                            mPhotobook.ExportPDFLibharu(path, exportName);
                        }
                    });
                }
            }
        }

        private bool StagedLineEmpty()
        {
            return StagedListView.Items.Count == 0;
        }

        private bool UnstagedLineEmpty()
        {
            return UnstagedListView.Items.Count == 0;
        }

        private UISelectionIndex GetSelectionIndex()
        {
            UISelectionIndex selectionIndex = new UISelectionIndex();

            var ranges = UnstagedListView.SelectedRanges;

            foreach (var range in ranges)
            {
                for (var i = range.FirstIndex; i <= range.LastIndex; i++)
                {
                    selectionIndex.UnstagedLineIndex.Add((uint)i);
                }
            }

            ranges = StagedListView.SelectedRanges;
            foreach (var range in ranges)
            {
                for (var i = range.FirstIndex; i <= range.LastIndex; i++)
                {
                    selectionIndex.StagedPhotoIndex.Add((uint)i);
                }
            }

            var index = MediaListView.SelectedIndex;
            if (index >= -1)
            {
                selectionIndex.ImportListIndex = (uint)index;
            }

            return selectionIndex;
        }

        private void UpdateUnstagedPhotoLine()
        {
            var selection = GetSelectionIndex();
            if (selection.ImportListIndex != null)
            {
                var iterator = mPhotobook.GetImageViews().ImageMonitor().StatefulIteratorByRow(selection.ImportListIndex.Value);
                if (iterator.Valid())
                {
                    var diff = mUnstagedImageCollection.Count - iterator.Size();
                    if (diff > 0)
                    {
                        for (var i = 0; i < diff; i++)
                        {
                            mUnstagedImageCollection.RemoveAt(mUnstagedImageCollection.Count - 1);
                        }
                    }
                    else if (diff < 0)
                    {
                        for (var i = 0; i < -diff; i++)
                        {
                            mUnstagedImageCollection.Add(new ImageUIData());
                        }
                    }

                    for (var i = 0; i < iterator.Size(); i++)
                    {
                        VirtualImagePtr virtualImage = iterator.At((uint)i).current();

                        mUnstagedImageCollection[i] = new ImageUIData(virtualImage.keyPath(),
                            virtualImage.frontend().fullPath(), virtualImage.frontend().mediumPath(),
                            virtualImage.frontend().smallPath(), virtualImage.processed());
                    }
                }
                else
                {
                    mUnstagedImageCollection.Clear();
                }
            }
        }

        /* Keyboard */
        private void OnKeyPressed(object sender, KeyRoutedEventArgs arg)
        {
            UISelectionIndex selectionIndex = GetSelectionIndex();

            if (selectionIndex.StagedPhotoIndex.Count == 0 && selectionIndex.UnstagedLineIndex.Count == 0)
            {
                return;
            }

            Windows.System.VirtualKey key = arg.Key;

            switch (key)
            {
                case Windows.System.VirtualKey.Delete:
                    {
                        var selection = GetSelectionIndex();
                        OnStagedImageRemoved(selection.StagedPhotoIndex);
                        mPhotobook.GetImageViews().StagedImages().RemovePicture(selection.StagedPhotoIndex);
                        break;
                    }
                default:
                    break;
            }
        }

        /* #18 */
        private void OnKeyDown(object sender, KeyRoutedEventArgs arg) { }

        protected override void OnKeyDown(KeyRoutedEventArgs arg)
        { }

        private void OnTableContentSizeChanged(object sender, SizeChangedEventArgs args)
        {
            UpdateCanvasSize();
            GalleryCanvas.Invalidate();
        }

        private double PaperToCanvasRatio(int width, int height,
                                            int boundingBoxWidth,
                                            int boundingBoxHeight)
        {
            System.Diagnostics.Debug.Assert(boundingBoxWidth > 0, "Width is 0");
            System.Diagnostics.Debug.Assert(boundingBoxHeight > 0, "Height is 0");

            double widthRatio = (double)width / boundingBoxWidth;
            double heightRatio = (double)height / boundingBoxHeight;

            double maxRatio = Math.Max(widthRatio, heightRatio);

            return maxRatio;
        }

        private void UpdateCanvasSize()
        {
            int width = (int)(CanvasBorder.ActualWidth - CanvasBorder.Padding.Left - CanvasBorder.Padding.Right);
            int height = (int)(CanvasBorder.ActualHeight - GalleryBottomName.ActualHeight - CanvasBorder.Padding.Top - CanvasBorder.Padding.Bottom);

            if (width > 0 && height > 0)
            {
                var paperSettings = mPhotobook.GetSettings().GetPaperSettings();
                double ratio = PaperToCanvasRatio(paperSettings.Width(), paperSettings.Height(),
                                      width, height);

                var newWidth = Math.Floor(paperSettings.Width() / ratio);
                var newHeight = Math.Floor(paperSettings.Height() / ratio);

                GalleryCanvas.Width = newWidth;
                GalleryCanvas.Height = newHeight;
            }
        }

        public void OnProjectRead()
        {
            // ILE
        }

        public void OnProjectRenamed()
        {
            // ILE
        }

        public void OnMetadataUpdated()
        {
            // ILE
        }

        public void OnPersistenceError(PBError error)
        {
            // ILE
        }

        public void OnExportFinished()
        {
            MainProgressBar.Visibility = Visibility.Collapsed;
        }

        public async void OnError(PBError error)
        {
            var errorString = error.ToString();
            GenericErrorTextBlock.Text = "Code: " + errorString;
            await GenericErrorDialog.ShowAsync();
        }

        public void OnStagedImageAdded(IList<VirtualImagePtr> photos, int index)
        {
            System.Diagnostics.Debug.Assert(index <= mStagedImageCollection.Count, "Index is greater than the count");
            if (index == mStagedImageCollection.Count || index < 0)
            {
                for (int i = 0; i < photos.Count; i++)
                {
                    var imageData = new ImageUIData(photos[i].keyPath(),
                                               photos[i].frontend().fullPath(),
                                               photos[i].frontend().mediumPath(),
                                               photos[i].frontend().smallPath(),
                                               photos[i].processed());
                    mStagedImageCollection.Add(imageData);
                }
            }
            else if (index < mStagedImageCollection.Count)
            {
                for (int i = 0; i < photos.Count; i++)
                {
                    var imageData = new ImageUIData(photos[i].keyPath(),
                                               photos[i].frontend().fullPath(),
                                               photos[i].frontend().mediumPath(),
                                               photos[i].frontend().smallPath(),
                                               photos[i].processed());
                    mStagedImageCollection.Insert(index, imageData);
                }
            }
        }

        public void OnStagedImageRemoved(IList<uint> removedIndexes)
        {
            // sort removedIndexes
            var sortedIndexes = removedIndexes.ToList();
            sortedIndexes.Sort();
            foreach (var index in sortedIndexes)
            {
                mStagedImageCollection.RemoveAt((int)index);
            }
            GalleryCanvas.Invalidate();
        }

        public void OnMappingStarted(string path)
        {
            // ILE
        }

        public void OnMappingFinished(string path)
        {
            mNavigationItemsCollection.Add(System.IO.Path.GetFileName(path));

            MediaListView.ItemsSource = mNavigationItemsCollection;
            MediaListView.SelectedIndex = mNavigationItemsCollection.Count - 1;
        }

        public void OnMappingAborted(string path)
        {
        }

        public void OnImageUpdated(string root, int row, int index)
        {
            var selection = GetSelectionIndex();
            var importedSelectedIndex = selection.ImportListIndex;
            if (importedSelectedIndex.HasValue && importedSelectedIndex.Value == row)
            {
                var virtualImage = mPhotobook.GetImageViews().ImageMonitor().Image((uint)row, (uint)index);
                mUnstagedImageCollection[(int)index] = new ImageUIData(virtualImage.keyPath(),
                                       virtualImage.frontend().fullPath(),
                                       virtualImage.frontend().mediumPath(),
                                       virtualImage.frontend().smallPath(),
                                       virtualImage.processed());
            }
        }

        public void OnProgressUpdate(ProgressInfo definedProgress, ProgressInfo undefinedProgress)
        {
            if (definedProgress.jobsProgress().Count == 0 || definedProgress.progressType() == ProgressType.None)
            {
                MainProgressBar.Visibility = Visibility.Collapsed;
            }
            else
            {
                MainProgressBar.Visibility = Visibility.Visible;
                MainProgressBar.Maximum = definedProgress.progressCap();
                MainProgressBar.Value = definedProgress.progress();
            }
            UpdateStatusBar(definedProgress, undefinedProgress);
        }

        public void OnPictureAdded(int index, int size)
        {
            throw new NotImplementedException();
        }

        public void OnPictureRemoved(IList<int> indices)
        {
            if (indices.Count == 0 || mPhotobook.GetImageViews().StagedImages().StagedPhotos().Count == 0)
            {
                return;
            }

            var selection = GetSelectionIndex();
            if (selection.StagedPhotoIndex.Count == 0)
            {
                var newSelectionIndex = Math.Max(indices[0] - 1, 0);
                StagedListView.SelectRange(new ItemIndexRange(newSelectionIndex, 1));
            }
        }
        // Rename this to clicked
        public async Task OnImportFolderAddedAsync()
        {
            await FireFolderPicker(onSuccess: (path) =>
            {
                mPhotobook.AddImportFolder(path);
            });
        }

        private void UpdateUnstagedLine()
        {
            var selection = GetSelectionIndex();
            mUnstagedImageCollection.Clear();
            if (selection.ImportListIndex != null)
            {
                var index = selection.ImportListIndex.Value;

                for (int i = 0; i < mPhotobook.GetImageViews().ImageMonitor().RowSize(index); i++)
                {
                    var image = mPhotobook.GetImageViews().ImageMonitor().Image(index, (uint)i);
                    mUnstagedImageCollection.Add(new ImageUIData(image.keyPath(),
                                               image.frontend().fullPath(),
                                               image.frontend().mediumPath(),
                                               image.frontend().smallPath(),
                                               image.processed()));
                }
            }
        }

        public void OnImportFolderRemoved(uint index)
        {
            MediaListView.DeselectRange(new ItemIndexRange(0, (uint)mNavigationItemsCollection.Count));
            mNavigationItemsCollection.RemoveAt((int)index);
            MediaListView.ItemsSource = mNavigationItemsCollection;
            if (mNavigationItemsCollection.Count > 0)
            {
                MediaListView.SelectedIndex = 0;
            }

            UpdateUnstagedLine();

        }

        public void OnRefresh()
        {
            LoadImages();
        }

        public void OnCleared()
        {
            //ILE
        }

        public void OnImportFolderAdded()
        {
        }

        public void Post(Functor f)
        {
            PhotobookSingletonWrapper.Inst().Post(() =>
            {
                f();
            });
        }
    }
}
