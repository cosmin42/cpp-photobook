#pragma once

#include <pb/DataManager.h>
#include <pb/Error.h>

namespace PB {

class GalleryListener final : public ImageSupportListener {
public:
  void setCallbacks(std::function<void(int)> onImportFolderAdded,
                    std::function<void()>    onStagePhotoUpdated)
  {
    mOnImportFolderAdded = onImportFolderAdded;
    mOnStagePhotoUpdated = onStagePhotoUpdated;
  }

  void importFolderAdded(int index) override
  {
    assert(mOnImportFolderAdded);
    mOnImportFolderAdded(index);
  }

  void stagePhotosUpdated() override
  {
    assert(mOnImportFolderAdded);
    mOnStagePhotoUpdated();
  }

private:
  std::function<void(int)> mOnImportFolderAdded = nullptr;
  std::function<void()>    mOnStagePhotoUpdated = nullptr;
};

class Gallery final {
public:
  Gallery(std::vector<Path> const &importedFolders)
      : mImportedFolders(importedFolders)
  {
    mGalleryListener = std::make_shared<GalleryListener>();
    mGalleryListener->setCallbacks(
        [](int) { PB::printDebug("Import folder added.\n"); },
        []() { PB::printDebug("Staged photos updated.\n"); });
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

  std::shared_ptr<ImageSupportListener> slot() { return mGalleryListener; }

private:
  std::shared_ptr<GalleryListener>          mGalleryListener = nullptr;
  int                                       mSelectedFolderIndex = -1;
  int                                       mGalleryIndex = -1;
  CircularIterator<std::vector<Thumbnails>> mCurrentIterator;
  std::vector<Path> const                  &mImportedFolders;
};
} // namespace PB