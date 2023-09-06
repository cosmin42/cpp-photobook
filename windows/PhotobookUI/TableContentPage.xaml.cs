using CommunityToolkit.Maui.Storage;
using CppWinRTProjection;
using System.Diagnostics;

namespace PhotobookUI;

public partial class TableContentPage : ContentPage
{
    private PhotoBook photoBook;
    public TableContentPage()
    {
        InitializeComponent();
        UIPhotoBookListener photoBookListener = new UIPhotoBookListener();
        photoBook = new PhotoBook(photoBookListener);
    }

    private async void OnBackClicked(object sender, EventArgs e)
    {
        await Navigation.PopAsync();
    }

    private async void OnAddMediaClicked(object sender, EventArgs e)
    {
        try
        {
            FolderPickerResult result = await FolderPicker.PickAsync(CancellationToken.None);

            if (result != null)
            {
                photoBook.AddMedia(result.Folder.Path);
            }
            else
            {
                Debug.Write("Folder was not chosen.");
            }
        }
        catch
        {
            Debug.WriteLine("Failed to choose folder.");
        }

        Debug.WriteLine("Add Media clicked.");
    }

}