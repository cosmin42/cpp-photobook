#include <pb/Gallery.h>

namespace PB {
Gallery::Gallery()
{
  mGalleryListener = std::make_shared<GalleryListener>();
  mGalleryListener->setCallbacks(
      [this](Path root, CircularIterator<std::vector<Thumbnails>> iterator) {
        mImportedFolders.push_back(root);
        mSelectedFolderIndex = (int)mImportedFolders.size() - 1;
        mCurrentIterator = iterator;
        PB::printDebug("Import folder added.\n");
      },
      [this](CircularIterator<std::vector<Thumbnails>> iterator) {
        mCurrentIterator = iterator;
        PB::printDebug("Staged photos updated.\n");
      });
}

void Gallery::setPosition(int position)
{
  if (mCurrentIterator.valid()) {
    mCurrentIterator = mCurrentIterator[position];
  }
}

void Gallery::navigateLeft()
{
  if (mCurrentIterator.valid()) {
    mCurrentIterator = mCurrentIterator.previous();
  }
}
void Gallery::navigateRight()
{
  if (mCurrentIterator.valid()) {
    mCurrentIterator = mCurrentIterator.next();
  }
}

void Gallery::selectImportFolder(
    int index, CircularIterator<std::vector<Thumbnails>> iterator)
{
  mSelectedFolderIndex = index;
  mCurrentIterator = iterator;
}

auto Gallery::selectedItem() -> std::optional<Thumbnails>
{
  if (mCurrentIterator.valid()) {
    return mCurrentIterator.current();
  }
  return std::nullopt;
}

std::shared_ptr<ImageSupportListener> Gallery::slot()
{
  return mGalleryListener;
}
} // namespace PB