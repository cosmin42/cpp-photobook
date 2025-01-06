using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace PhotobookNet
{
    sealed class UiSelectionData
    {
        public UiSelectionData()
        {
            ImportListIndex = null;
            UnstagedLineIndex = [];
            StagedPhotoIndex = [];
        }

        // Maybe the selected index
        public uint? ImportListIndex { set; get; }

        // The UPL selected list
        public List<uint> UnstagedLineIndex { set; get; }

        // The SPL selected list
        public List<uint> StagedPhotoIndex { set; get; }
    };
}
