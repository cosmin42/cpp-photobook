#pragma once

#include <pb/DataManager.h>
#include <pb/Error.h>
#include <pb/GalleryListener.h>

namespace PB {

template <typename PhotoBookType, typename T>
  requires TaskManageableConcept<PhotoBookType>
class Gallery final {
public:
  Gallery(GalleryListener<PhotoBookType, T> &listener)
      : mListener(listener)
  {
  }
  ~Gallery() = default;

  void selectIndex(int newIndex)
  {
    mSelectedFolderIndex = newIndex;

    mCurrentMap = mediaMap(mSelectedFolderIndex);

    if (mCurrentMap) {
      mCurrentIterator = mCurrentMap->iterator();
    }
    else {
      mCurrentIterator = std::nullopt;
    }
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
    auto &mediaIndexedByType = Context::inst().data().mediaIndexedByType();

    assert(mSelectedFolderIndex < mediaIndexedByType.size() &&
           mSelectedFolderIndex > -1);
    return mediaIndexedByType.at(mSelectedFolderIndex);
  }

  auto selectedItem() -> std::optional<Path>
  {
    if (mCurrentIterator.has_value()) {
      return mCurrentIterator->current();
    }
    return std::nullopt;
  }

  auto foldersList() -> std::vector<Path>
  {
    return Context::inst().data().mediaIndexedByType();
  }

private:
  auto mediaMap(unsigned index) -> std::optional<MediaMap>
  {
    auto &mediaIndexedByType = Context::inst().data().mediaIndexedByType();
    if (index >= mediaIndexedByType.size()) {
      return std::nullopt;
    }

    auto &key = mediaIndexedByType.at(index);

    auto &mediaData = Context::inst().data().mediaData();
    if (!mediaData.contains(key)) {
      return std::nullopt;
    }

    return mediaData.at(key);
  }

  GalleryListener<PhotoBookType, T> &mListener;

  int                             mSelectedFolderIndex = -1;
  int                             mGalleryIndex = -1;
  std::optional<MediaMap>         mCurrentMap;
  std::optional<CircularIterator> mCurrentIterator;
};
} // namespace PB