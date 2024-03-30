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
    /// An empty page that can be used on its own or navigated to within a Frame.
    /// </summary>
    public sealed partial class TableContentPage : Page, PhotobookListener
    {

        //ObservableCollection<ProjectItem> mProjectsList;
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

        private static (int, int) ScreenSize()
        {
            var bounds = Window.Current.Bounds;
            int width = (int)bounds.Width;
            int height = (int)bounds.Height;
            return (width, height);
        }

        public TableContentPage()
        {
            this.InitializeComponent();
            PhotobookSingletonWrapper.GetInstance().ConfigurePhotobookListener(this);
        }

        private int CanvasMinWidth
        {
            get
            {
                var paperSettings = PhotobookSingletonWrapper.GetInstance().GetSettings().GetPaperSettings();
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
                var paperSettings = PhotobookSingletonWrapper.GetInstance().GetSettings().GetPaperSettings();
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

        private void PostponeError(string message)
        {
            GenericErrorTextBlock.Text = message;
            Post(() =>
            {
                GenericErrorDialog.ShowAsync();
            });
        }

        /* Menu Bar */
        private async void OnExportClicked(object sender, RoutedEventArgs args)
        {
            if (mStagedImageCollection.Count == 0)
            {
                PostponeError("There is no staged photo!");
            }
            else
            {
                Post(() => { ExportContentDialog.ShowAsync(); });
            }
        }

        private async void OnBackClicked(object sender, RoutedEventArgs args)
        {
            var isSaved = PhotobookSingletonWrapper.GetInstance().GetSettings().IsSaved(PhotobookSingletonWrapper.GetInstance().GetImageViews().ImageMonitor().Unstaged(),
                PhotobookSingletonWrapper.GetInstance().GetImageViews().StagedImages().StagedPhotos(),
                PhotobookSingletonWrapper.GetInstance().GetImageViews().ImageMonitor().RowList());
            if (!isSaved)
            {
                mBackFlag = true;
                SaveProjectDialog.ShowAsync();
            }
            else
            {
                PhotobookSingletonWrapper.GetInstance().UnloadProject();
                Frame.Navigate(typeof(MainWindow));
            }
        }

        private async void OnAboutClicked(object sender, RoutedEventArgs args)
        {
            GenericMessageTextBlock.Text = "Photobook v1.0\n\nDeveloped by:\n\n- A\n- B\n- C\n- D\n- E\n- F\n- G\n- H\n- I\n- J\n- K\n- L\n- M\n- N\n- O\n- P\n- Q\n- R\n- S\n- T\n- U\n- V\n- W\n- X\n- Y\n- Z";
            Post(() =>
            {
                GnericMessage.ShowAsync();
            });
        }

        private async void OnLicenseClicked(object sender, RoutedEventArgs args)
        {
            // TODO: Use a resx file
        }

        private async void OnTipsClicked(object sender, RoutedEventArgs args) { }

        private async void OnExitClicked(object sender, RoutedEventArgs args)
        {
            Post(() =>
            {
                Microsoft.UI.Xaml.Application.Current.Exit();
            });
            mExitFlag = true;
        }

        private async void OnSaveClicked(object sender, RoutedEventArgs args)
        {
            PhotobookSingletonWrapper.GetInstance().GetSettings().Save(PhotobookSingletonWrapper.GetInstance().GetSettings().CurrentProjectUUID(),
                PhotobookSingletonWrapper.GetInstance().GetImageViews().ImageMonitor().Unstaged(),
                PhotobookSingletonWrapper.GetInstance().GetImageViews().StagedImages().StagedPhotos(),
                PhotobookSingletonWrapper.GetInstance().GetImageViews().ImageMonitor().RowList());
        }

        private async void OnSaveAsClicked(object sender, RoutedEventArgs args)
        {
            Post(() => { RenameProjectDialog.ShowAsync(); });
        }

        private async void OnNewClicked(object sender, RoutedEventArgs args)
        {
            var isSaved = PhotobookSingletonWrapper.GetInstance().GetSettings().IsSaved(PhotobookSingletonWrapper.GetInstance().GetImageViews().ImageMonitor().Unstaged(),
                PhotobookSingletonWrapper.GetInstance().GetImageViews().StagedImages().StagedPhotos(),
                PhotobookSingletonWrapper.GetInstance().GetImageViews().ImageMonitor().RowList());

            if (isSaved)
            {
                PhotobookSingletonWrapper.GetInstance().UnloadProject();
                Frame.Navigate(typeof(MainWindow), "new-project");
            }
            else
            {
                mNewProjectFlag = true;
                SaveProjectDialog.ShowAsync();
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
                imagePtr = PhotobookSingletonWrapper.GetInstance().GetImageViews().StagedImages().Picture((int)selection.StagedPhotoIndex[0]);
            }
            else if (selection.UnstagedLineIndex.Count != 0)
            {
                imagePtr = PhotobookSingletonWrapper.GetInstance().GetImageViews().ImageMonitor().Image((uint)selection.ImportListIndex.Value, (uint)selection.UnstagedLineIndex[0]);
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

        private async void OnUndoClicked(object sender, RoutedEventArgs args) { }

        private async void OnRedoClicked(object sender, RoutedEventArgs args) { }

        private async void OnPreferencesClicked(object sender, RoutedEventArgs args) { }

        /* Navigation */

        private async Task FireFolderPicker(Action<string> onSuccess)
        {
            Windows.Storage.Pickers.FolderPicker folderPicker = new Windows.Storage.Pickers.FolderPicker();
            folderPicker.FileTypeFilter.Add("*");
            var folder = await folderPicker.PickSingleFolderAsync();
            onSuccess(folder.Path);
        }

        private async void OnImportFolderAdded(object sender, RoutedEventArgs args)
        {
            await FireFolderPicker(onSuccess: (path) =>
            {
                Post(() =>
                {
                    PhotobookSingletonWrapper.GetInstance().AddImportFolder(path);
                });
            });
        }

        private async void OnImportFolderRemoved(object sender, RoutedEventArgs args)
        {
            var selection = MediaListView.SelectedIndex;
            System.Diagnostics.Debug.Assert(selection >= 0, "Selection is less than 0");
            var rowPath = PhotobookSingletonWrapper.GetInstance().GetImageViews().ImageMonitor().RowPath((uint)selection);
            PhotobookSingletonWrapper.GetInstance().RemoveImportFolder(rowPath);
        }

        private async void OnImportSelectionChanged(object sender, SelectionChangedEventArgs args)
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
                PhotobookSingletonWrapper.GetInstance().GetImageViews().StagedImages().PopImages((int)changedIndex);
            }
            else if (changeType == Windows.Foundation.Collections.CollectionChange.ItemRemoved)
            {
                PhotobookSingletonWrapper.GetInstance().GetImageViews().StagedImages().StashImages(new List<uint> { (uint)changedIndex });
            }
            else
            {
                System.Diagnostics.Debug.Assert(false, "Invalid change type");
            }
        }

        /* Gallery */
        private async void OnGalleryLeft(object sender, RoutedEventArgs args)
        {
            Left();
            UpdateGalleryLabel();
        }

        private async void OnGalleryRight(object sender, RoutedEventArgs args)
        {
            Right();
            UpdateGalleryLabel();
        }

        private async void OnCanvasDraw(CanvasControl sender, CanvasDrawEventArgs args)
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
        }

        /* Book Lines */

        protected override void OnNavigatedTo(NavigationEventArgs args)
        {
            if (UnstagedListView.Items.Count > 0)
            {
                UnstagedListView.SelectRange(new ItemIndexRange(0, 1));
            }
            else if (StagedListView.Items.Count > 0)
            {
                StagedListView.SelectRange(new ItemIndexRange(0, 1));
            }
        }



        private async void OnUnstagedPhotosSelectionChanged(object sender, SelectionChangedEventArgs args)
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

        private async void OnStagedPhotosSelectionChanged(object sender, SelectionChangedEventArgs args)
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

        private async void OnStagedDragItemsStarting(object sender, DragItemsStartingEventArgs args)
        {
            mDragSource = DragSource.Staged;
        }

        private async void OnStagedDragItemsCompleted(object sender, DragItemsCompletedEventArgs args)
        {
            if (mDragSource == DragSource.Staged)
            {
                mDragSource = DragSource.None;
            }
        }

        private async void OnUnstagedPhotosDragStarted(object sender, DragItemsStartingEventArgs args)
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
                var imagePtr = PhotobookSingletonWrapper.GetInstance().GetImageViews().ImageMonitor().Image(keyPath);
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

        private async void OnDropIntoStagedPhotos(object sender, DragEventArgs args)
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

        private async void OnDragOverStagedPhotos(object sender, DragEventArgs args)
        {
            args.AcceptedOperation = Windows.ApplicationModel.DataTransfer.DataPackageOperation.Copy;
        }

        private async void OnClickedOutsideList(object sender, TappedRoutedEventArgs args)
        {

        }

        private async void UnstagedSelectAllInvoked(KeyboardAccelerator sender, KeyboardAcceleratorInvokedEventArgs args)
        {
            UnstagedListView.SelectAll();
        }

        /* Dialogs - Save */
        private async void OnContentDialogDiscardClicked(object sender, ContentDialogButtonClickEventArgs args) {
            PhotobookSingletonWrapper.GetInstance().UnloadProject();
            if (mExitFlag)
            {
                Post(() => { Microsoft.UI.Xaml.Application.Current.Exit(); });
            }
            else
            {
                Frame.Navigate(typeof(MainWindow));
            }
        }

        private async void OnContentDialogCancelClicked(object sender, ContentDialogButtonClickEventArgs args) {
            
        }

        private async void OnRenameProjectDialogRename(object sender, ContentDialogButtonClickEventArgs args) {
            PhotobookSingletonWrapper.GetInstance().GetSettings().Rename(RenameProjectDialogTextBox.Text, "");
        }

        private async void OnRenameProjectDialogCancel(object sender, ContentDialogButtonClickEventArgs args) { 
            if (mExitFlag)
            {
                Post(() => { Microsoft.UI.Xaml.Application.Current.Exit(); });
            }
        }

        private async void OnSaveProject(object sender, ContentDialogButtonClickEventArgs args) {
            PhotobookSingletonWrapper.GetInstance().GetSettings().Save(PhotobookSingletonWrapper.GetInstance().GetSettings().CurrentProjectUUID(),
                PhotobookSingletonWrapper.GetInstance().GetImageViews().ImageMonitor().Unstaged(),
                PhotobookSingletonWrapper.GetInstance().GetImageViews().StagedImages().StagedPhotos(),
                PhotobookSingletonWrapper.GetInstance().GetImageViews().ImageMonitor().RowList());
            if(mExitFlag)
            {
                Post(() => { Microsoft.UI.Xaml.Application.Current.Exit(); });
            }
            if(mNewProjectFlag)
            {
                mNewProjectFlag = false;
                PhotobookSingletonWrapper.GetInstance().UnloadProject();
                Frame.Navigate(typeof(MainWindow), "new-project");
            }
            if (mBackFlag)
            {
                mBackFlag = false;
                PhotobookSingletonWrapper.GetInstance().UnloadProject();
                Frame.Navigate(typeof(MainWindow));
            }
        }


        private async void OnCancelSavingProject(object sender, ContentDialogButtonClickEventArgs args) {
            if (mExitFlag)
            {
                Post(() => { Microsoft.UI.Xaml.Application.Current.Exit(); });
            }
            if (mNewProjectFlag)
            {
                mNewProjectFlag = false;
                PhotobookSingletonWrapper.GetInstance().UnloadProject();
                Frame.Navigate(typeof(MainWindow), "new-project");
            }
            if (mBackFlag)
            {
                mBackFlag = false;
                PhotobookSingletonWrapper.GetInstance().UnloadProject();
                Frame.Navigate(typeof(MainWindow));
            }
        }

        /* Dialogs - Export */
        private async void OnExportContentDialogClicked(object sender, ContentDialogButtonClickEventArgs args)
        {
            var exportName = ExportNameTextBox.Text;

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
                var iterator = PhotobookSingletonWrapper.GetInstance().GetImageViews().ImageMonitor().StatefulIteratorByRow(selection.ImportListIndex.Value);
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
        private async void OnKeyPressed(object sender, KeyRoutedEventArgs arg)
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
                        PhotobookSingletonWrapper.GetInstance().GetImageViews().StagedImages().RemovePicture(selection.StagedPhotoIndex);
                        break;
                    }
                default:
                    break;
            }
        }

        /* #18 */
        private async void OnKeyDown(object sender, KeyRoutedEventArgs arg) { }

        protected override async void OnKeyDown(KeyRoutedEventArgs arg)
        { }

        private async void OnTableContentSizeChanged(object sender, SizeChangedEventArgs args)
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
                var paperSettings = PhotobookSingletonWrapper.GetInstance().GetSettings().GetPaperSettings();
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
            throw new System.NotImplementedException();
        }

        public void OnProjectRenamed()
        {
        }

        public void OnMetadataUpdated()
        {
            throw new System.NotImplementedException();
        }

        public void OnPersistenceError(PBError error)
        {
            throw new System.NotImplementedException();
        }

        public void OnExportFinished()
        {
            throw new System.NotImplementedException();
        }

        public void OnError(PBError error)
        {
            throw new System.NotImplementedException();
        }

        public void OnStagedImageAdded(IList<VirtualImagePtr> photos, int index)
        {
            throw new System.NotImplementedException();
        }

        public void OnStagedImageRemoved(IList<uint> removedIndexes)
        {
            throw new System.NotImplementedException();
        }

        public void OnMappingStarted(string path)
        {
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
            throw new System.NotImplementedException();
        }

        public void OnProgressUpdate(ProgressInfo definedProgress, ProgressInfo undefinedProgress)
        {
            throw new System.NotImplementedException();
        }
        public void Post(Action function)
        {
            bool success = Microsoft.UI.Dispatching.DispatcherQueue.GetForCurrentThread().TryEnqueue(() =>
            {
                function();
            });
            System.Diagnostics.Debug.Assert(success, "Navigation to TableContentPage failed");
        }
    }
}
