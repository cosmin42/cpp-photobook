#pragma once

#include <ranges>
#include <unordered_map>
#include <unordered_set>

#include <pb/Config.h>
#include <pb/PreprocessedImage.h>
#include <pb/VirtualImage.h>
#include <pb/util/CircularIterator.h>

namespace PB {

class ImageSupportListener {
public:
  virtual void importFolderAdded(
      Path root,
      CircularIterator<std::vector<std::shared_ptr<VirtualImage>>>) = 0;
  virtual void stagePhotosUpdated(
      CircularIterator<std::vector<std::shared_ptr<VirtualImage>>>) = 0;
};

class ImageSupport final {
public:
  void setListener(std::shared_ptr<ImageSupportListener> listener);

  void addGroup(std::optional<Path> path);

  void addImage(Path root, std::vector<Path> paths,
                std::vector<std::shared_ptr<VirtualImage>>);

  std::vector<Path>   fullPathByGroup(Path group);
  std::optional<Path> groupByIndex(int index);

  void stagePhoto(std::vector<std::shared_ptr<VirtualImage>> fullPath,
                  int                                        position = -1);
  void unstagePhoto(std::vector<int> importedFolderIndex);
  void removeGroup(int index);

  int fullPathRow(Path) const;

  void clear();

  auto stagedIterator()
      -> CircularIterator<std::vector<std::shared_ptr<VirtualImage>>>;
  auto unstagedIterator(Path root)
      -> CircularIterator<std::vector<std::shared_ptr<VirtualImage>>>;
  auto unstagedIterator(int importFolderIndex, int index = -1)
      -> CircularIterator<std::vector<std::shared_ptr<VirtualImage>>>;

  int                                  groupSize(std::optional<Path> group);
  std::unordered_map<Path, int> const &groups();
  std::vector<std::shared_ptr<VirtualImage>> &stagedPhotos();
  std::vector<Path>                           stagedPhotosFullPaths() const;

  template <typename T> std::shared_ptr<T> image(Path fullPath)
  {
    auto &[importPathIndex, pathIndex] = mSupportByFullPath.at(fullPath);
    return std::dynamic_pointer_cast<T>(
        mUnstagedImagesMatrix.at(importPathIndex).at(pathIndex));
  }

private:
  std::unordered_map<Path, int>                           mGroupIndexes;
  std::unordered_map<Path, std::pair<int, int>>           mSupportByFullPath;
  std::vector<std::vector<std::shared_ptr<VirtualImage>>> mUnstagedImagesMatrix;
  std::vector<std::shared_ptr<VirtualImage>>              mStagedPhotos;
  std::shared_ptr<ImageSupportListener>                   mListener = nullptr;
};

} // namespace PB