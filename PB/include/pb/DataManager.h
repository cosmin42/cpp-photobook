#pragma once

#include <ranges>
#include <unordered_map>

#include <pb/MediaMap.h>
#include <pb/util/CircularIterator.h>

namespace PB {

struct Thumbnails {
  Thumbnails(Path path, int newIndex) : fullPath(path), index{newIndex} {}
  const int index;
  Path      fullPath;
  Path      mediumThumbnail;
  Path      smallThumbnail;
};

class ImageSupport final {
public:
  void addFullPaths(Path root, std::vector<Path> const &paths);
  void addSmall(Path fullSize, Path smallSize);
  void addMedium(Path fullSize, Path mediumSize);
  std::vector<Path>   fullPathByGroup(Path group);
  std::optional<Path> groupByIndex(int index);

  auto thumbnailsSet(Path root)
  {
    auto filterFunction = [this, root{root}](Thumbnails const &th) {
      if (mGroupContent.find(root) == mGroupContent.end()) {
        return false;
      }
      return std::find(mGroupContent.at(root).begin(),
                       mGroupContent.at(root).end(),
                       th.index) != mGroupContent.at(root).end();
    };

    return CircularIterator<std::vector<Thumbnails>>(mSupport, filterFunction);
  }
  int                       groupSize(std::optional<Path> group);
  std::optional<Thumbnails> getByMedium(std::optional<Path> path);
  void                      addGroup(std::optional<Path> path, unsigned size);
  void                      clear();
  std::vector<Path> const  &groups();

private:
  std::unordered_map<Path, std::vector<int>> mGroupContent;
  std::vector<Path>                          mGroup;
  std::unordered_map<Path, int>              mSupportBySmallThumbnail;
  std::unordered_map<Path, int>              mSupportByMediumThumbnail;
  std::unordered_map<Path, int>              mSupportByFullPath;
  std::vector<Thumbnails>                    mSupport;
};

class DataManager final {
public:
  ImageSupport &images() { return mImages; }

private:
  ImageSupport mImages;
};
} // namespace PB