#pragma once

#include <ranges>
#include <unordered_map>

#include <pb/MediaMap.h>
#include <pb/util/CircularIterator.h>

namespace PB {

struct Thumbnails {
  explicit Thumbnails(Path path) : fullPath(path) {}
  explicit Thumbnails(Path fPath, Path mPath, Path sPath)
      : fullPath(fPath), mediumThumbnail(mPath), smallThumbnail(sPath)
  {
  }
  Path fullPath;
  Path mediumThumbnail;
  Path smallThumbnail;
};

class ImageSupport final {
public:
  void                addFullPaths(Path root, std::vector<Path> const &paths);
  void                addSmall(Path fullSize, Path smallSize);
  void                addMedium(Path fullSize, Path mediumSize);
  std::vector<Path>   fullPathByGroup(Path group);
  std::optional<Path> groupByIndex(int index);

  auto thumbnailsSet(Path root) -> CircularIterator<std::vector<Thumbnails>>;
  int  groupSize(std::optional<Path> group);
  std::optional<Thumbnails> getByMedium(std::optional<Path> path);
  std::optional<Thumbnails> getBySmall(std::optional<Path> path);
  void                      addGroup(std::optional<Path> path, unsigned size);
  void                      clear();
  std::vector<Path> const  &groups();

  void stagePhoto(Thumbnails fullPath, int position = -1);
  void unstagePhoto(int index);

  std::vector<Thumbnails> &stagedPhotos() { return mStagedPhotos; }

  auto stagedIterator() -> CircularIterator<std::vector<Thumbnails>>;

private:
  std::unordered_map<Path, std::vector<int>> mGroupContent;
  std::vector<Path>                          mGroup;
  std::unordered_map<Path, int>              mSupportBySmallThumbnail;
  std::unordered_map<Path, int>              mSupportByMediumThumbnail;
  std::unordered_map<Path, int>              mSupportByFullPath;
  std::vector<Thumbnails>                    mSupport;
  std::vector<Thumbnails>                    mStagedPhotos;
};

class DataManager final {
public:
  ImageSupport &images() { return mImages; }

private:
  ImageSupport mImages;
};
} // namespace PB