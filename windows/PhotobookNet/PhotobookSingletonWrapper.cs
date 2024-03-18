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
        private static PhotobookWin instance = null;

        private PhotobookSingletonWrapper() { }

        public static PhotobookWin GetInstance()
        {
            if (instance == null)
            {
                var localFolderPath = Windows.Storage.ApplicationData.Current.LocalFolder.Path;
                var installFolderPath = Windows.ApplicationModel.Package.Current.InstalledLocation.Path;
                instance = new PhotobookWin(localFolderPath.ToString(), installFolderPath.ToString());
            }
            return instance;
        }
    }
}
