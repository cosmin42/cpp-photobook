using CppWinRTProjection;

namespace PhotobookUI
{
    public partial class MainPage : ContentPage, CppWinRTProjection.GradualControllableListener
    {
        public MainPage()
        {
            InitializeComponent();
        }

        private async void OnNewProjectClicked(object sender, EventArgs e)
        {
            await Navigation.PushAsync(new TableContentPage());
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
}