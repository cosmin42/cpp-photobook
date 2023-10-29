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
    PB::basicAssert(mOnImportFolderAdded != nullptr);
    mOnImportFolderAdded(root, iterator);
  }

  void stagePhotosUpdated(
      CircularIterator<std::vector<Thumbnails>> iterator) override
  {
    PB::basicAssert(mOnImportFolderAdded != nullptr);
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
  void selectImportFolder(int                                       index,
                          CircularIterator<std::vector<Thumbnails>> iterator);

  void selectStagedPhotos(int                                       index,
                          CircularIterator<std::vector<Thumbnails>> iterator);

  void clearSelection();

  auto selectedItem() -> std::optional<Thumbnails>;
  auto selectedIndex() -> int;

  auto photoLine() const -> PhotoLine;

  std::shared_ptr<ImageSupportListener> slot();

private:
  std::shared_ptr<GalleryListener>          mGalleryListener = nullptr;
  int                                       mSelectedFolderIndex = -1;
  CircularIterator<std::vector<Thumbnails>> mCurrentIterator;
  std::vector<Path>                         mImportedFolders;
  PhotoLine                                 mPhotoLine = PhotoLine::None;
};
} // namespace PB