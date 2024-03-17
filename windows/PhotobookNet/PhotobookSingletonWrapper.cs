using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using PhotobookRuntimeComponent;

namespace PhotobookNet
{
    class PhotobookSingletonWrapper
    {
        private static PhotobookWin instance;

        private PhotobookSingletonWrapper() { }

        public static PhotobookWin GetInstance()
        {
            return instance;
        }
    }
}
