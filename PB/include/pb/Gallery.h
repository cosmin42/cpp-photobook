#pragma once

#include <pb/DataManager.h>
#include <pb/Error.h>

namespace PB {

class GalleryListener final : public ImageSupportListener {
public:
  void setCallbacks(
      std::function<void(Path, CircularIterator<std::vector<Thumbnails>>)>
          onImportFolderAdded,
      std::function<void(CircularIterator<std::vector<Thumbnails>>)>
          onStagePhotoUpdated)
  {
    mOnImportFolderAdded = onImportFolderAdded;
    mOnStagePhotoUpdated = onStagePhotoUpdated;
  }

  void
  importFolderAdded(Path                                      root,
                    CircularIterator<std::vector<Thumbnails>> iterator) override
  {
    assert(mOnImportFolderAdded);
    mOnImportFolderAdded(root, iterator);
  }

  void stagePhotosUpdated(
      CircularIterator<std::vector<Thumbnails>> iterator) override
  {
    assert(mOnImportFolderAdded);
    mOnStagePhotoUpdated(iterator);
  }

private:
  std::function<void(Path, CircularIterator<std::vector<Thumbnails>>)>
      mOnImportFolderAdded = nullptr;
  std::function<void(CircularIterator<std::vector<Thumbnails>>)>
      mOnStagePhotoUpdated = nullptr;
};

class Gallery final {
public:
  Gallery(std::vector<Path> const &importedFolders)
      : mImportedFolders(importedFolders)
  {
    mGalleryListener = std::make_shared<GalleryListener>();
    mGalleryListener->setCallbacks(
        [this](Path root, CircularIterator<std::vector<Thumbnails>>) {
          mImportedFolders.push_back(root);
          PB::printDebug("Import folder added.\n");
        },
        [](CircularIterator<std::vector<Thumbnails>>) {
          PB::printDebug("Staged photos updated.\n");
        });
  }

  ~Gallery() = default;

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

  auto selectedImportFolder() -> std::optional<Path>
  {
    if (mSelectedFolderIndex >= mImportedFolders.size() ||
        mSelectedFolderIndex < 0) {
      return std::nullopt;
    }
    return mImportedFolders.at(mSelectedFolderIndex);
  }

  auto selectedItem() -> std::optional<Thumbnails>
  {
    if (mCurrentIterator.valid()) {
      return mCurrentIterator.current();
    }
    return std::nullopt;
  }

  std::shared_ptr<ImageSupportListener> slot() { return mGalleryListener; }

private:
  std::shared_ptr<GalleryListener>          mGalleryListener = nullptr;
  int                                       mSelectedFolderIndex = -1;
  int                                       mGalleryIndex = -1;
  CircularIterator<std::vector<Thumbnails>> mCurrentIterator;
  std::vector<Path>                         mImportedFolders;
};
} // namespace PB