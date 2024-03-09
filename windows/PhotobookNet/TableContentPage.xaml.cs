using Microsoft.UI.Xaml;
using Microsoft.UI.Xaml.Controls;
using Microsoft.UI.Xaml.Input;
using Microsoft.UI.Xaml.Navigation;
using Microsoft.Graphics.Canvas.UI.Xaml;


// To learn more about WinUI, the WinUI project structure,
// and more about our project templates, see: http://aka.ms/winui-project-info.

namespace PhotobookNet
{
    /// <summary>
    /// An empty page that can be used on its own or navigated to within a Frame.
    /// </summary>
    public sealed partial class TableContentPage : Page
    {
        public TableContentPage()
        {
            this.InitializeComponent();
        }

        /* Menu Bar */
        private async void OnExportClicked(object sender, RoutedEventArgs args) { }

        private async void OnBackClicked(object sender, RoutedEventArgs args) { }

        private async void OnAboutClicked(object sender, RoutedEventArgs args) { }

        private async void OnLicenseClicked(object sender, RoutedEventArgs args) { }

        private async void OnTipsClicked(object sender, RoutedEventArgs args) { }

        private async void OnExitClicked(object sender, RoutedEventArgs args) { }

        private async void OnSaveClicked(object sender, RoutedEventArgs args) { }

        private async void OnSaveAsClicked(object sender, RoutedEventArgs args) { }

        private async void OnNewClicked(object sender, RoutedEventArgs args) { }

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

        protected override async void OnNavigatedTo(NavigationEventArgs args) { }

        private async void OnUnstagedPhotosSelectionChanged(object sender, SelectionChangedEventArgs args) { }

        private async void OnStagedPhotosSelectionChanged(object sender, SelectionChangedEventArgs args) { }

        private async void OnStagedDragItemsStarting(object sender, DragItemsStartingEventArgs args) { }

        private async void OnStagedDragItemsCompleted(object sender, DragItemsCompletedEventArgs args) { }

        private async void OnUnstagedPhotosDragStarted(object sender, DragItemsStartingEventArgs args) { }

        private async void OnDropIntoStagedPhotos(object sender, DragEventArgs args) { }

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
        private async void OnExportContentDialogClicked(object sender, ContentDialogButtonClickEventArgs args) { }

        /* Keyboard */
        private async void OnKeyPressed(object sender, KeyRoutedEventArgs arg) { }

        /* #18 */
        private async void OnKeyDown(object sender, KeyRoutedEventArgs arg) { }

        protected override async void OnKeyDown(KeyRoutedEventArgs arg)
        { }

        private async void OnTableContentSizeChanged(object sender, SizeChangedEventArgs args)
        { }

        int CanvasMinWidth { get; set; }
        int CanvasMinHeight { get; set; }
    }
}
