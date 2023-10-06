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
  void                addFullPaths(Path root, std::vector<Path> const &paths);
  void                addSmall(Path fullSize, Path smallSize);
  void                addMedium(Path fullSize, Path mediumSize);
  std::vector<Path>   fullPathByGroup(Path group);
  std::optional<Path> groupByIndex(int index);

  auto thumbnailsSet(Path root)
  {
    auto filterFunction = [this, root{root}](Thumbnails const &th) {
      if (mGroupContent.find(root) == mGroupContent.end()) {
        return false;
      }
      return mGroupContent.at(root).find(th.index) !=
             mGroupContent.at(root).end();
    };
    auto result = std::ranges::filter_view(mSupport,
            filterFunction);

    return CircularIterator<decltype(result)>(result);
  }
  int                       groupSize(std::optional<Path> group);
  std::optional<Thumbnails> getByMedium(std::optional<Path> path);
  void                      addGroup(std::optional<Path> path);
  void                      clear();
  std::vector<Path> const  &groups();

private:
  std::unordered_map<Path, std::set<int>> mGroupContent;
  std::vector<Path>                       mGroup;
  std::unordered_map<Path, int>           mSupportBySmallThumbnail;
  std::unordered_map<Path, int>           mSupportByMediumThumbnail;
  std::unordered_map<Path, int>           mSupportByFullPath;
  std::vector<Thumbnails>                 mSupport;
};

typedef decltype(ImageSupport().thumbnailsSet(Path())) FilteredThumbnailSet;

class DataManager final {
public:
  ImageSupport &images() { return mImages; }

private:
  ImageSupport mImages;
};
} // namespace PB