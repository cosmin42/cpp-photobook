#pragma once

#include <pb/DataManager.h>
#include <pb/Error.h>

namespace PB {

class GalleryListener final : public ImageSupportListener {
public:
  void setCallbacks(
      std::function<void(
          Path, CircularIterator<std::vector<std::shared_ptr<VirtualImage>>>)>
          onImportFolderAdded,
      std::function<
          void(CircularIterator<std::vector<std::shared_ptr<VirtualImage>>>)>
          onStagePhotoUpdated)
  {
    mOnImportFolderAdded = onImportFolderAdded;
    mOnStagePhotoUpdated = onStagePhotoUpdated;
  }

  void
  importFolderAdded(Path root,
                    CircularIterator<std::vector<std::shared_ptr<VirtualImage>>>
                        iterator) override
  {
    // Forward
    PB::basicAssert(mOnImportFolderAdded != nullptr);
    mOnImportFolderAdded(root, iterator);
  }

  void stagePhotosUpdated(
      CircularIterator<std::vector<std::shared_ptr<VirtualImage>>> iterator)
      override
  {
    // Forward
    PB::basicAssert(mOnImportFolderAdded != nullptr);
    mOnStagePhotoUpdated(iterator);
  }

private:
  std::function<void(
      Path, CircularIterator<std::vector<std::shared_ptr<VirtualImage>>>)>
      mOnImportFolderAdded = nullptr;
  std::function<void(
      CircularIterator<std::vector<std::shared_ptr<VirtualImage>>>)>
      mOnStagePhotoUpdated = nullptr;
};

class Gallery final {
public:
  Gallery();
  ~Gallery() = default;

  void selectImportFolder(
      int                                                          index,
      CircularIterator<std::vector<std::shared_ptr<VirtualImage>>> iterator);

  void selectStagedPhotos(
      int                                                          index,
      CircularIterator<std::vector<std::shared_ptr<VirtualImage>>> iterator);

  void setPhotoLinePosition(int position);
  void clearSelection();

  std::shared_ptr<ImageSupportListener> slot();

  auto selectedItem() const -> std::shared_ptr<VirtualImage>;
  auto selectedNavigationIndex() const -> int;
  auto photoLine() const -> PhotoLine;
  auto hasSelection() const -> bool;

private:
  std::shared_ptr<GalleryListener> mGalleryListener = nullptr;
  int                              mSelectedFolderIndex = -1;
  CircularIterator<std::vector<std::shared_ptr<VirtualImage>>> mCurrentIterator;
  std::vector<Path>                                            mImportedFolders;
  PhotoLine mPhotoLine = PhotoLine::None;
};
} // namespace PB