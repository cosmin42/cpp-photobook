#pragma once

#include <pb/DataManager.h>
#include <pb/Error.h>
#include <pb/GalleryListener.h>

namespace PB {

template <typename PhotoBookListenerType, typename PersistenceType>
  requires PhotoBookListenerConcept<PhotoBookListenerType>
class Gallery final {
public:
  Gallery(GalleryListener<PhotoBookListenerType, PersistenceType> &listener,
          std::vector<Path> const &importedFolders)
      : mListener(listener), mImportedFolders(importedFolders)
  {
  }
  ~Gallery() = default;

  void setIterator(CircularIterator<std::vector<Thumbnails>> iterator,
                   int                                       position = 0)
  {
    mSelectedFolderIndex = position;
    mCurrentIterator = iterator;
    setPosition(position);
  }

  void setPosition(int position)
  {
    if (mCurrentIterator.valid()) {
      mCurrentIterator = mCurrentIterator[position];
    }
  }

  void navigateLeft()
  {
    if (mCurrentIterator.valid()) {
      mCurrentIterator = mCurrentIterator.previous();
    }
  }
  void navigateRight()
  {
    if (mCurrentIterator.valid()) {
      mCurrentIterator = mCurrentIterator.next();
    }
  }

  auto folderName() -> std::optional<Path>
  {
    assert(mSelectedFolderIndex < mImportedFolders.size() &&
           mSelectedFolderIndex > -1);
    return mImportedFolders.at(mSelectedFolderIndex);
  }

  auto selectedItem() -> std::optional<Thumbnails>
  {
    if (mCurrentIterator.valid()) {
      return mCurrentIterator.current();
    }
    return std::nullopt;
  }

private:
  int mSelectedFolderIndex = -1;
  int mGalleryIndex = -1;
  GalleryListener<PhotoBookListenerType, PersistenceType> &mListener;
  CircularIterator<std::vector<Thumbnails>>                mCurrentIterator;
  std::vector<Path> const                                 &mImportedFolders;
};
} // namespace PB