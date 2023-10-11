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
  Gallery();
  ~Gallery() = default;

  void setPosition(int position);
  void navigateLeft();
  void navigateRight();
  void selectImportFolder(int                                       index,
                          CircularIterator<std::vector<Thumbnails>> iterator);

  auto selectedItem() -> std::optional<Thumbnails>;

  std::shared_ptr<ImageSupportListener> slot();

private:
  std::shared_ptr<GalleryListener>          mGalleryListener = nullptr;
  int                                       mSelectedFolderIndex = -1;
  int                                       mGalleryIndex = -1;
  CircularIterator<std::vector<Thumbnails>> mCurrentIterator;
  std::vector<Path>                         mImportedFolders;
};
} // namespace PB