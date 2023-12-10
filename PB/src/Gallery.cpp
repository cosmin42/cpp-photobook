#include <pb/Gallery.h>

namespace PB {
/*
Gallery::Gallery()
{
  mGalleryListener = std::make_shared<GalleryListener>();
  mGalleryListener->setCallbacks(
      [this](int newSize,
             CircularIterator<std::vector<std::shared_ptr<VirtualImage>>>
                 iterator) {
        mSelectedFolderIndex = newSize;
        mCurrentIterator = iterator;
        PB::printDebug("Import folder added.\n");
      },
      [this](CircularIterator<std::vector<std::shared_ptr<VirtualImage>>>
                 iterator) {
        mCurrentIterator = iterator;
        PB::printDebug("Staged photos updated.\n");
      });
}

void Gallery::setPhotoLinePosition(int position)
{
  if (mCurrentIterator.valid()) {
    mCurrentIterator = mCurrentIterator[position];
  }
}

void Gallery::selectImportFolder(
    int                                                          index,
    CircularIterator<std::vector<std::shared_ptr<VirtualImage>>> iterator)
{
  mPhotoLine = PhotoLine::Unstaged;
  mSelectedFolderIndex = index;
  mCurrentIterator = iterator;
}

void Gallery::selectStagedPhotos(
    int                                                          index,
    CircularIterator<std::vector<std::shared_ptr<VirtualImage>>> iterator)
{
  mPhotoLine = PhotoLine::Staged;
  mSelectedFolderIndex = index;
  mCurrentIterator = iterator;
}

void Gallery::clearSelection()
{
  mGalleryListener = nullptr;
  mSelectedFolderIndex = -1;
  mCurrentIterator =
      CircularIterator<std::vector<std::shared_ptr<VirtualImage>>>();
  mPhotoLine = PhotoLine::None;
}

auto Gallery::selectedNavigationIndex() const -> int
{
  return mSelectedFolderIndex;
}

auto Gallery::hasSelection() const -> bool { return mCurrentIterator.valid(); }

auto Gallery::selectedItem() const -> std::shared_ptr<VirtualImage>
{
  if (mCurrentIterator.valid()) {
    return mCurrentIterator.current();
  }
  return nullptr;
}

auto Gallery::photoLine() const -> PhotoLine { return mPhotoLine; }

std::shared_ptr<ImageSupportListener> Gallery::slot()
{
  return mGalleryListener;
}
*/
} // namespace PB