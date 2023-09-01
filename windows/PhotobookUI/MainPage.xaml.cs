using CppWinRTProjection;

namespace PhotobookUI
{
    public partial class MainPage
    {
        public MainPage()
        {
            InitializeComponent();
        }

        private async void OnNewProjectClicked(object sender, EventArgs e)
        {
            await Navigation.PushAsync(new TableContentPage());
        }

        
    }
}