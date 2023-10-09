#pragma once

#include <pb/DataManager.h>
#include <pb/Error.h>
#include <pb/GalleryListener.h>

namespace PB {

template <typename PhotoBookType, typename PersistenceType>
  requires PhotoBookListenerConcept<PhotoBookType>
class Gallery final {
public:
  Gallery(GalleryListener<PhotoBookType, PersistenceType> &listener)
      : mListener(listener)
  {
  }
  ~Gallery() = default;

  void setIterator(CircularIterator<std::vector<Thumbnails>> iterator)
  {
    mSelectedFolderIndex = 0;
    mCurrentIterator = iterator;
  }

  void setPosition(int position)
  {
    mCurrentIterator = mCurrentIterator->goToPosition(position);
  }

  void navigateLeft()
  {
    if (mCurrentIterator) {
      mCurrentIterator = mCurrentIterator->previous();
    }
  }
  void navigateRight()
  {
    if (mCurrentIterator) {
      mCurrentIterator = mCurrentIterator->next();
    }
  }

  auto folderName() -> std::optional<Path>
  {
    auto &mediaIndexedByType = Context::inst().data().images().groups();
    assert(mSelectedFolderIndex < mediaIndexedByType.size() &&
           mSelectedFolderIndex > -1);
    return mediaIndexedByType.at(mSelectedFolderIndex);
  }

  auto selectedItem() -> std::optional<Thumbnails>
  {
    if (mCurrentIterator.has_value()) {
      return mCurrentIterator->current();
    }
    return std::nullopt;
  }

  auto foldersList() -> std::vector<Path>
  {
    return Context::inst().data().images().groups();
  }

private:
  GalleryListener<PhotoBookType, PersistenceType> &mListener;
  int                                              mSelectedFolderIndex = -1;
  int                                              mGalleryIndex = -1;
  std::optional<CircularIterator<std::vector<Thumbnails>>> mCurrentIterator =
      std::nullopt;
};
} // namespace PB