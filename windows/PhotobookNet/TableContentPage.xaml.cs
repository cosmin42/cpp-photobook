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

        int CanvasMinWidth { get; set; }

        int CanvasMinHeight { get; set; }

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

        public TableContentPage()
        {
            this.InitializeComponent();
            PhotobookSingletonWrapper.GetInstance().ConfigurePhotobookListener(this);
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
                ExportContentDialog.ShowAsync();
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

        private async void OnUndoClicked(object sender, RoutedEventArgs args) { }

        private async void OnRedoClicked(object sender, RoutedEventArgs args) { }

        private async void OnPreferencesClicked(object sender, RoutedEventArgs args) { }

        /* Navigation */
        private async void OnImportFolderAdded(object sender, RoutedEventArgs args) { }

        private async void OnImportFolderRemoved(object sender, RoutedEventArgs args) { }

        private async void OnImportSelectionChanged(object sender, SelectionChangedEventArgs args)
        { }

        /* Gallery */
        private async void OnGalleryLeft(object sender, RoutedEventArgs args) { }

        private async void OnGalleryRight(object sender, RoutedEventArgs args) { }
        private async void OnCanvasDraw(CanvasControl sender, CanvasDrawEventArgs args) { }

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



        private async void OnUnstagedPhotosSelectionChanged(object sender, SelectionChangedEventArgs args) { }

        private async void OnStagedPhotosSelectionChanged(object sender, SelectionChangedEventArgs args) { }

        private async void OnStagedDragItemsStarting(object sender, DragItemsStartingEventArgs args) { }

        private async void OnStagedDragItemsCompleted(object sender, DragItemsCompletedEventArgs args) { }

        private async void OnUnstagedPhotosDragStarted(object sender, DragItemsStartingEventArgs args) { }

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

        private async void OnDragOverStagedPhotos(object sender, DragEventArgs args) { }

        private async void OnClickedOutsideList(object sender, TappedRoutedEventArgs args) { }

        private async void UnstagedSelectAllInvoked(KeyboardAccelerator sender, KeyboardAcceleratorInvokedEventArgs args) { }

        /* Dialogs - Save */
        private async void OnContentDialogDiscardClicked(object sender, ContentDialogButtonClickEventArgs args) { }

        private async void OnContentDialogCancelClicked(object sender, ContentDialogButtonClickEventArgs args) { }

        private async void OnRenameProjectDialogRename(object sender, ContentDialogButtonClickEventArgs args) { }

        private async void OnRenameProjectDialogCancel(object sender, ContentDialogButtonClickEventArgs args) { }

        private async void OnSaveProject(object sender, ContentDialogButtonClickEventArgs args) { }


        private async void OnCancelSavingProject(object sender, ContentDialogButtonClickEventArgs args) { }

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
            throw new System.NotImplementedException();
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
            throw new System.NotImplementedException();
        }

        public void OnMappingFinished(string path)
        {
            throw new System.NotImplementedException();
        }

        public void OnMappingAborted(string path)
        {
            throw new System.NotImplementedException();
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
