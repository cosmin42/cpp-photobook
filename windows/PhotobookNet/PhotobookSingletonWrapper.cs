using System;
using System.Diagnostics;
using System.Linq;
using PhotobookRuntimeComponent;
using WindowsDisplayAPI;

namespace PhotobookNet
{
    class PhotobookSingletonWrapper
    {
        private static PhotobookSingletonWrapper sPhotobookSingletonWrapper = null;

        private PhotobookWin mPhotobook = null;
        private Microsoft.UI.Dispatching.DispatcherQueue mMainDispatcherQueue;
        private Action mOnWindowClosed = null;
        private Action<string> mChangeTitleHander = null;

        private nint mWindowHandle;

        private PhotobookSingletonWrapper()
        {
            var localFolderPath = Windows.Storage.ApplicationData.Current.LocalFolder.Path;
            var installFolderPath = Windows.ApplicationModel.Package.Current.InstalledLocation.Path;
            var screenSize = GetBiggestScreenResolution();
            mPhotobook = new PhotobookWin(localFolderPath.ToString(), installFolderPath.ToString(), screenSize);
        }

        internal Int32Pair GetBiggestScreenResolution()
        {
            uint maxDisplaySurface = 0;
            Int32Pair maxResolution = new Int32Pair(0, 0);

            Debug.Assert(Display.GetDisplays().Count() > 0, "No displays found.");

            foreach (Display display in Display.GetDisplays())
            {
                var resolution = display.CurrentSetting.Resolution;

                if (resolution.Width * resolution.Height > maxDisplaySurface)
                {
                    maxResolution = new Int32Pair(resolution.Width, resolution.Height);
                    maxDisplaySurface = (uint)(resolution.Width * resolution.Height);
                }
            }
            return maxResolution;
        }

        public void SetWindowHandle(nint handle)
        {
            mWindowHandle = handle;
        }

        public void SetChangeTitleHandler(Action<string> changeTitleHandler)
        {
            mChangeTitleHander = changeTitleHandler;
        }

        public void UpdateTitle(string projectName = null)
        {
            string titleText = "Photobook Noir";
            if (!string.IsNullOrEmpty(projectName))
            {
                titleText += " - " + projectName;
            }
            mChangeTitleHander(titleText);
        }

        public void SetThisThreadAsMainDispatcher()
        {
            mMainDispatcherQueue = Microsoft.UI.Dispatching.DispatcherQueue.GetForCurrentThread();
        }

        public PhotobookWin Photobook()
        {
            return mPhotobook;
        }

        public nint GetWindowHandle()
        {
            return mWindowHandle;
        }

        public void Post(Action function)
        {
            bool success = mMainDispatcherQueue.TryEnqueue(() =>
            {
                function();
            });
            System.Diagnostics.Debug.Assert(success, "Navigation to TableContentPage failed");
        }

        public void SetOnWindowClosed(Action onWindowClosed)
        {
            mOnWindowClosed = onWindowClosed;
        }

        public void OnWindowClosed()
        {
            mOnWindowClosed();
        }

        public static PhotobookSingletonWrapper Inst()
        {
            if (sPhotobookSingletonWrapper == null)
            {
                sPhotobookSingletonWrapper = new PhotobookSingletonWrapper();
            }
            return sPhotobookSingletonWrapper;
        }
    }
}
