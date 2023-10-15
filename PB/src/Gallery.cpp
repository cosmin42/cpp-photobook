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
  mPhotoLine = PhotoLine::Unstaged;
  mSelectedFolderIndex = index;
  mCurrentIterator = iterator;
}

void Gallery::selectStagedPhotos(
    CircularIterator<std::vector<Thumbnails>> iterator)
{
  mPhotoLine = PhotoLine::Staged;
  mSelectedFolderIndex = -1;
  mCurrentIterator = iterator;
}

void Gallery::clearSelection()
{
  mGalleryListener = nullptr;
  mSelectedFolderIndex = -1;
  mCurrentIterator = CircularIterator<std::vector<Thumbnails>>();
  mImportedFolders.clear();
  mPhotoLine = PhotoLine::None;
}

auto Gallery::selectedIndex() -> int { return mSelectedFolderIndex; }

auto Gallery::selectedItem() -> std::optional<Thumbnails>
{
  if (mCurrentIterator.valid()) {
    return mCurrentIterator.current();
  }
  return std::nullopt;
}

auto Gallery::photoLine() const -> PhotoLine { return mPhotoLine; }

std::shared_ptr<ImageSupportListener> Gallery::slot()
{
  return mGalleryListener;
}
} // namespace PB