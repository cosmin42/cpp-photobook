using CommunityToolkit.Maui.Storage;
using System.Diagnostics;

namespace PhotobookUI;

public partial class TableContentPage : ContentPage
{
    public TableContentPage()
    {
        InitializeComponent();
    }

    private async void OnBackClicked(object sender, EventArgs e)
    {
        await Navigation.PopAsync();
    }

    private async void OnAddMediaClicked(object sender, EventArgs e)
    {

        try
        {
            var result = await FolderPicker.PickAsync(CancellationToken.None);

            if (result != null)
            {
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