using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using PhotobookRuntimeComponent;
using Windows.Gaming.XboxLive;

namespace PhotobookNet
{
    class PhotobookSingletonWrapper
    {
        private static PhotobookSingletonWrapper sPhotobookSingletonWrapper = null;

        private PhotobookWin mPhotobook = null;
        private Microsoft.UI.Dispatching.DispatcherQueue mMainDispatcherQueue;
        private Action mOnWindowClosed = null;

        private nint mWindowHandle;

        private PhotobookSingletonWrapper()
        {
            var localFolderPath = Windows.Storage.ApplicationData.Current.LocalFolder.Path;
            var installFolderPath = Windows.ApplicationModel.Package.Current.InstalledLocation.Path;
            mPhotobook = new PhotobookWin(localFolderPath.ToString(), installFolderPath.ToString(), new Int32Pair(2560, 1440));
        }

        public void SetWindowHandle(nint handle)
        {
            mWindowHandle = handle;
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
