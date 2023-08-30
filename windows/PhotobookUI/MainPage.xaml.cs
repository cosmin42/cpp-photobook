using CppWinRTProjection;

namespace PhotobookUI
{
    public partial class MainPage : ContentPage, CppWinRTProjection.GradualControllableListener
    {
        int count = 0;

        public MainPage()
        {
            InitializeComponent();
        }

        private void OnCounterClicked(object sender, EventArgs e)
        {
            count++;

            if (count == 1)
                CounterBtn.Text = $"Clicked {count} time";
            else
                CounterBtn.Text = $"Clicked {count} times";

            SemanticScreenReader.Announce(CounterBtn.Text);
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