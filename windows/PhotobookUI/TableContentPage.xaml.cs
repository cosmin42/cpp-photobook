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

}