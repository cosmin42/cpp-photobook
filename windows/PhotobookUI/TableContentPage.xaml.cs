using CommunityToolkit.Maui.Storage;
using System.Diagnostics;

namespace PhotobookUI;

public partial class TableContentPage : ContentPage, CppWinRTProjection.GradualControllableListener
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

    public void OnFinished()
    {
        throw new NotImplementedException();
    }

    public void OnPaused()
    {
        throw new NotImplementedException();
    }

    public void OnProgressUpdate()
    {
        throw new NotImplementedException();
    }

    public void OnResumed()
    {
        throw new NotImplementedException();
    }

    public void OnStarted()
    {
        throw new NotImplementedException();
    }

    public void OnStopped()
    {
        throw new NotImplementedException();
    }

}