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
        private static readonly PhotobookSingletonWrapper sPhotobookSingletonWrapper = null;

        private PhotobookWin mPhotobook = null;
        private Microsoft.UI.Dispatching.DispatcherQueue mMainDispatcherQueue;

        private PhotobookSingletonWrapper()
        {
            var localFolderPath = Windows.Storage.ApplicationData.Current.LocalFolder.Path;
            var installFolderPath = Windows.ApplicationModel.Package.Current.InstalledLocation.Path;
            mPhotobook = new PhotobookWin(localFolderPath.ToString(), installFolderPath.ToString());
        }

        public void SetThisThreadAsMainDispatcher()
        {
            mMainDispatcherQueue = Microsoft.UI.Dispatching.DispatcherQueue.GetForCurrentThread();
        }

        public PhotobookWin Photobook()
        {
            return mPhotobook;
        }

        public void Post(Action function)
        {
            bool success = mMainDispatcherQueue.TryEnqueue(() =>
            {
                function();
            });
            System.Diagnostics.Debug.Assert(success, "Navigation to TableContentPage failed");
        }

        public static PhotobookSingletonWrapper Inst()
        {
            return sPhotobookSingletonWrapper;
        }
    }
}
