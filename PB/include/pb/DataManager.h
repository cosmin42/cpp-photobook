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
  void removeGroup(int index);

  std::optional<Path> groupByIndex(int index) const;
  int                 groupSize(std::optional<Path> group) const;
  std::vector<Path>   groups() const;
  int                 groupIndex(Path path) const;
  bool                containsGroup(Path path);

  void addImage(Path root, std::vector<Path> paths,
                std::vector<std::shared_ptr<VirtualImage>>);

  int fullPathRow(Path) const;

  void stagePhoto(std::vector<std::shared_ptr<VirtualImage>> photo,
                  int                                        position = -1);
  void updateStagedPhoto(std::shared_ptr<VirtualImage> newImage);
  void unstagePhoto(std::vector<int> importedFolderIndex);
  std::vector<std::shared_ptr<VirtualImage>> const &stagedPhotos();

  void clear();

  auto stagedIterator()
      -> CircularIterator<std::vector<std::shared_ptr<VirtualImage>>>;
  auto unstagedIterator(Path root)
      -> CircularIterator<std::vector<std::shared_ptr<VirtualImage>>>;
  auto unstagedIterator(int importFolderIndex, int index = -1)
      -> CircularIterator<std::vector<std::shared_ptr<VirtualImage>>>;

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