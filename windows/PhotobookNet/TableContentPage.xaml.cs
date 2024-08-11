using Microsoft.UI.Xaml;
using Microsoft.UI.Xaml.Controls;
using Microsoft.UI.Xaml.Input;
using Microsoft.UI.Xaml.Navigation;
using Microsoft.Graphics.Canvas.UI.Xaml;
using PhotobookRuntimeComponent;
using System.Collections.Generic;
using Microsoft.UI.Xaml.Data;
using System.Collections.ObjectModel;
using System;
using System.Threading.Tasks;
using System.Collections.Specialized;
using System.Linq;
using Microsoft.Graphics.Canvas;


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
        ObservableCollection<VirtualImagePtr> mUnstagedImageCollection;
        ObservableCollection<VirtualImagePtr> mStagedImageCollection;
        Collection<VirtualImagePtr> mDragAndDropSelectedImages;
        bool mExitFlag = false;
        bool mNewProjectFlag = false;
        DragSource mDragSource = DragSource.None;
        bool mLinesExclusiveSelection = false;
        bool mBackFlag = false;
        bool mJustRemove = false;
        bool doNothing = false;

        (int, int) frameSize = (438, 310);

        IDictionary<Guid, uint> mSPLProcessingImages = new Dictionary<Guid, uint>();

        private PhotobookWin mPhotobook;

        private PhotobookRuntimeComponent.PaperSettings PaperSettingsCache;

        public TableContentPage()
        {
            mPhotobook = PhotobookSingletonWrapper.Inst().Photobook();

            PaperSettingsCache = mPhotobook.GetSettings().GetPaperSettings();

            frameSize = (PaperSettingsCache.Width, PaperSettingsCache.Height);

            this.InitializeComponent();

            mCollageIconsPaths = new ObservableCollection<CollageTemplateInfo>();

            mDragAndDropSelectedImages = new Collection<VirtualImagePtr>();

            mPhotobook.ConfigurePhotobookListener(this);

            mPhotobook.ConfigureStagedImagesListener(this);
            mPhotobook.ConfigureImageMonitorListener(this);

            mNavigationItemsCollection = new ObservableCollection<string>();
            mUnstagedImageCollection = new ObservableCollection<VirtualImagePtr>();
            mStagedImageCollection = new ObservableCollection<VirtualImagePtr>();

            CollageTemplatesGridView.ItemsSource = mCollageIconsPaths;

            var collageButtonsEnabled = CollageTemplatesGridView.SelectedItems.Count() > 0;
            PreviewAppBarButton.IsEnabled = collageButtonsEnabled;
            MakeCollage.IsEnabled = collageButtonsEnabled;

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
                    if (doNothing)
                    {
                        doNothing = false;
                        return;
                    }
                    mPhotobook.GetImageViews().StagedImages().PopImages(args.NewStartingIndex);

                }
                else if (args.Action == NotifyCollectionChangedAction.Remove)
                {
                    if (mJustRemove)
                    {
                        mJustRemove = false;
                    }
                    else
                    {
                        // TODO: Change signature to accept single index.
                        mPhotobook.GetImageViews().StagedImages().StashImages(new List<uint> { (uint)args.OldStartingIndex });
                    }
                }
                else if (args.Action == NotifyCollectionChangedAction.Move)
                {
                    System.Diagnostics.Debug.Assert(false, "Invalid change type");
                }
            };

            mUnstagedImageCollection.CollectionChanged += (object sender, NotifyCollectionChangedEventArgs args) =>
            {
                if ((sender as ObservableCollection<VirtualImagePtr>).Count == 0)
                {
                    RemoveMediaAppBarButton.IsEnabled = false;

                }
                else
                {
                    RemoveMediaAppBarButton.IsEnabled = true;
                }
            };
        }

        private int CanvasMinWidth
        {
            get
            {
                System.Diagnostics.Debug.Assert(PaperSettingsCache.Ppi > 0, "Width is 0");
                double ratio = PaperToCanvasRatio(PaperSettingsCache.Width, PaperSettingsCache.Height, 438, 310);

                if (ratio > 1)
                {
                    return (int)Math.Floor(PaperSettingsCache.Width / ratio);
                }
                else
                {
                    return PaperSettingsCache.Width;
                }
            }
            set { }
        }

        private int CanvasMinHeight
        {
            get
            {
                System.Diagnostics.Debug.Assert(PaperSettingsCache.Ppi > 0, "Height is 0");
                double ratio = PaperToCanvasRatio(PaperSettingsCache.Width, PaperSettingsCache.Height, 438, 310);

                if (ratio > 1)
                {
                    return (int)Math.Floor(PaperSettingsCache.Height / ratio);
                }
                else
                {
                    return PaperSettingsCache.Height;
                }
            }
            set { }
        }

        private void CanvasGridSizeChanged(object sender, SizeChangedEventArgs e)
        {
            double ratio = PaperToCanvasRatio(frameSize.Item1, frameSize.Item2, CanvasGridName.ActualWidth, CanvasGridName.ActualHeight);

            GalleryCanvas.Width = (int)Math.Floor(frameSize.Item1 / ratio);
            GalleryCanvas.Height = (int)Math.Floor(frameSize.Item2 / ratio);
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
                var lastROwIndex = rowList.Count - 1;

                for (int i = 0; i < mPhotobook.GetImageViews().ImageMonitor().RowSize((uint)lastROwIndex); i++)
                {
                    var image = mPhotobook.GetImageViews().ImageMonitor().Image((uint)lastROwIndex, (uint)i);
                    mUnstagedImageCollection.Insert(i, image);
                }

                MediaListView.SelectedIndex = mNavigationItemsCollection.Count - 1;
            }

            var stagedPictures = mPhotobook.GetImageViews().StagedImages().StagedPhotos();

            for (int i = 0; i < stagedPictures.Count; ++i)
            {
                doNothing = true;
                mStagedImageCollection.Add(stagedPictures[i]);
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

        private void OnPropertiesClicked(object sender, RoutedEventArgs args)
        {
            PhotobookSingletonWrapper.Inst().Post(async () => { await PropertiesContentDialog.ShowAsync(); });
            PropertiesContentTextBlock.Loaded += (object sender, RoutedEventArgs args) =>
            {
                PropertiesContentTextBlock.Text = "Paper size: " + PaperSettingsCache.Width.ToString() + "x" + PaperSettingsCache.Height.ToString() + "px\n" +
                    "PPI: " + PaperSettingsCache.Ppi.ToString() + "\n" +
                    "Paper type: " + PaperSettingsCache.Type.ToString() + "\n";
            };
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
            else if (selection.UnstagedLineIndex.Count != 0 && selection.ImportListIndex.HasValue)
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

            GalleryMainText.Text = System.IO.Path.GetFileName(imagePtr.Frontend.FullPath);

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

        private void OnMakeCollageClick(object sender, RoutedEventArgs args)
        {
            var selection = GetSelectionIndex();

            var selectedImages = selection.StagedPhotoIndex;

            var slectedCollageIndex = (uint)CollageTemplatesGridView.SelectedRanges.First().FirstIndex;

            mPhotobook.MakeCollage(selectedImages, slectedCollageIndex);
        }


        private async void OnAddMediaClick(object sender, RoutedEventArgs args)
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

        private void OnCollageSelectionChanged(object sender, SelectionChangedEventArgs args)
        {
            var collageButtonsEnabled = CollageTemplatesGridView.SelectedItems.Count() > 0;
            if (collageButtonsEnabled)
            {
                var selection = GetSelectionIndex();
                bool imagesAreSelected = selection.StagedPhotoIndex.Count > 0;

                if (imagesAreSelected)
                {
                    var imageCount = (args.AddedItems.First() as CollageTemplateInfo).ImageCount;

                    uint imagesLeftOutside = (uint)selection.StagedPhotoIndex.Count % imageCount;

                    collageButtonsEnabled = imagesAreSelected && imagesLeftOutside == 0;
                }
                else
                {
                    collageButtonsEnabled = false;
                }
            }

            PreviewAppBarButton.IsEnabled = collageButtonsEnabled;
            MakeCollage.IsEnabled = collageButtonsEnabled;
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
            // TODO Add managed implementation.

            VirtualImagePtr imagePtr = null;

            if (selection.UnstagedLineIndex.Count != 0)
            {
                imagePtr = mUnstagedImageCollection[(int)selection.UnstagedLineIndex[0]];
            }
            else if (selection.StagedPhotoIndex.Count != 0)
            {
                imagePtr = mStagedImageCollection[(int)selection.StagedPhotoIndex[0]];
            }

            System.Diagnostics.Debug.Assert(imagePtr != null);

            frameSize = (imagePtr.Size().First, imagePtr.Size().Second);

            double ratio = PaperToCanvasRatio(frameSize.Item1, frameSize.Item2, CanvasGridName.ActualWidth, CanvasGridName.ActualHeight);

            GalleryCanvas.Width = (int)Math.Floor(frameSize.Item1 / ratio);
            GalleryCanvas.Height = (int)Math.Floor(frameSize.Item2 / ratio);

            int portviewWidth = (int)GalleryCanvas.Width;
            int portviewHeight = (int)GalleryCanvas.Height;

            // TODO: make this static as it is always used.
            byte[] byteArray = new byte[portviewWidth * portviewHeight * 4];

            imagePtr.GalleryProjection(byteArray, portviewWidth, portviewHeight);

            CanvasDevice device = CanvasDevice.GetSharedDevice();

            CanvasBitmap bitmap = Microsoft.Graphics.Canvas.CanvasBitmap.CreateFromBytes(device, byteArray, portviewWidth, portviewHeight, Windows.Graphics.DirectX.DirectXPixelFormat.B8G8R8X8UIntNormalized);

            session.DrawImage(bitmap);
        }

        /* Book Lines */

        public ObservableCollection<CollageTemplateInfo> mCollageIconsPaths { get; set; }

        protected override void OnNavigatedTo(NavigationEventArgs args)
        {
            LoadImages();
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

            var selection = GetSelectionIndex();
            bool imagesAreSelected = selection.StagedPhotoIndex.Count > 0;
            if (imagesAreSelected)
            {

                for (var i = 0; i < mCollageIconsPaths.Count; ++i)
                {
                    uint imagesLeftOutside = (uint)selection.StagedPhotoIndex.Count % mCollageIconsPaths[i].ImageCount;
                    bool isTemplateFitting = imagesLeftOutside == 0;
                    if (!isTemplateFitting)
                    {
                        mCollageIconsPaths[i].Info = "Select a multiple of " + mCollageIconsPaths[i].ImageCount;
                    }
                    else
                    {
                        mCollageIconsPaths[i].Info = "";
                    }
                    mCollageIconsPaths[i] = mCollageIconsPaths[i];
                }
            }
            else
            {
                for (var i = 0; i < mCollageIconsPaths.Count; ++i)
                {
                    mCollageIconsPaths[i].Info = "";
                    mCollageIconsPaths[i] = mCollageIconsPaths[i];
                }
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
                var image = item as VirtualImagePtr;
                if (!image.Processed())
                {
                    allowDrag = false;
                    break;
                }
                var keyPath = image.Frontend.FullPath;
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

            OnStagedImageAdded(mDragAndDropSelectedImages, insertPosition);

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

        private void saveProject()
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

        private void OnSaveProject(object sender, RoutedEventArgs args)
        {
            saveProject();
        }

        private void OnSaveProject(object sender, ContentDialogButtonClickEventArgs args)
        {
            saveProject();
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
                            mPhotobook.ExportPDFAlbum(exportName, path);
                        }
                        if (exportSelection.Contains(ExportType.Jpg))
                        {
                            mPhotobook.ExportJPGAlbum(exportName, path);
                        }
                        if (exportSelection.Contains(ExportType.PdfLibharu))
                        {
                            mPhotobook.ExportPDFLibharu(exportName, path);
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
            if (index > -1)
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
                            var emptyImage = mPhotobook.EmptyImage();
                            mUnstagedImageCollection.Add(emptyImage);
                        }
                    }

                    for (var i = 0; i < iterator.Size(); i++)
                    {
                        VirtualImagePtr virtualImage = iterator.At((uint)i).current();

                        mUnstagedImageCollection[i] = virtualImage;
                    }
                }
                else
                {
                    mUnstagedImageCollection.Clear();
                }
            }
        }

        public void OnCollageThumbnailsCreated()
        {
            mCollageIconsPaths.Clear();

            var thumbnailsList = mPhotobook.CollageTemplatesThumbnailsList();

            foreach (var thumbnail in thumbnailsList)
            {
                mCollageIconsPaths.Add(thumbnail);
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
                                            double boundingBoxWidth,
                                            double boundingBoxHeight)
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
                double ratio = PaperToCanvasRatio(PaperSettingsCache.Width, PaperSettingsCache.Height,
                                      width, height);

                var newWidth = Math.Floor(PaperSettingsCache.Width / ratio);
                var newHeight = Math.Floor(PaperSettingsCache.Height / ratio);

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
            var errorString = error.description();
            GenericErrorTextBlock.Text = "Code: " + errorString;
            await GenericErrorDialog.ShowAsync();
        }

        public void OnStagedImageAdded(IList<VirtualImagePtr> photos, int index)
        {
            System.Diagnostics.Debug.Assert(index <= mStagedImageCollection.Count, "Index is greater than the count");

            Dictionary<Guid, VirtualImagePtr> imagesToBeProcessed = new Dictionary<Guid, VirtualImagePtr>();

            if (index == mStagedImageCollection.Count || index < 0)
            {
                var newImages = new List<VirtualImagePtr>();
                for (int i = 0; i < photos.Count; i++)
                {
                    var newImage = mPhotobook.EmptyImage();
                    newImages.Add(newImage);
                    doNothing = true;
                    mStagedImageCollection.Add(newImage);

                    var imageId = Guid.NewGuid();
                    imagesToBeProcessed[imageId] = photos[i];
                    mSPLProcessingImages[imageId] = (uint)i;
                }
                mPhotobook.GetImageViews().StagedImages().AddPictures(newImages, (int)index);
            }
            else if (index < mStagedImageCollection.Count)
            {
                var newImages = new List<VirtualImagePtr>();
                for (int i = 0; i < photos.Count; i++)
                {
                    var newImage = mPhotobook.EmptyImage();
                    doNothing = true;
                    newImages.Add(newImage);
                    mStagedImageCollection.Insert(index, newImage);

                    var imageId = Guid.NewGuid();
                    imagesToBeProcessed[imageId] = photos[i];
                    mSPLProcessingImages[imageId] = (uint)i;
                }
                mPhotobook.GetImageViews().StagedImages().AddPictures(newImages, (int)index);
            }

            if (imagesToBeProcessed.Count > 0)
            {
                mPhotobook.mapImagesToSPL(imagesToBeProcessed);
            }
        }

        public void OnStagedImageRemoved(IList<uint> removedIndexes)
        {
            // sort removedIndexes in decreasing order
            var sortedIndexes = removedIndexes.OrderByDescending(i => i).ToList();

            foreach (var index in sortedIndexes)
            {
                mJustRemove = true;
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
                mUnstagedImageCollection[(int)index] = virtualImage;
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
            // TODO: Add unused parameter
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
                    mUnstagedImageCollection.Add(image);
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

        public void OnCollageCreated(uint index, VirtualImagePtr newImage)
        {
            doNothing = true;
            var newImages = new List<VirtualImagePtr>();
            newImages.Add(newImage);
            mPhotobook.GetImageViews().StagedImages().AddPictures(newImages, (int)index);
            mStagedImageCollection.Insert((int)index, newImage);
        }

        public void OnImageMapped(Guid imageId, VirtualImagePtr image)
        {
            var splIndex = mSPLProcessingImages[imageId];

            mStagedImageCollection[(int)splIndex] = image;

            mSPLProcessingImages.Remove(imageId);

            mPhotobook.GetImageViews().StagedImages().RemovePicture(new List<uint> { splIndex });
            mPhotobook.GetImageViews().StagedImages().AddPictures(new List<VirtualImagePtr> { image }, (int)splIndex);
        }
    }
}
