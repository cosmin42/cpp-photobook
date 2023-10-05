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

  std::vector<Path> fullPathByGroup(Path group)
  {
    if (mGroupContent.find(group) == mGroupContent.end()) {
      return std::vector<Path>();
    }
    auto             &pathSet = mGroupContent.at(group);
    std::vector<Path> result;
    for (auto index : pathSet) {
      result.push_back(mSupport.at(index).fullPath);
    }
    return result;
  }

  std::optional<Path> groupByIndex(int index)
  {
    if (index > 1 && index < mGroup.size()) {
      return mGroup.at(index);
    }
    return std::nullopt;
  }

  auto thumbnailsSet(Path root)
  {
    auto result =
        mSupport |
        std::ranges::views::filter([this, root{root}](Thumbnails const &th) {
          if (mGroupContent.find(root) == mGroupContent.end()) {
            return false;
          }
          return mGroupContent.at(root).find(th.index) !=
                 mGroupContent.at(root).end();
        });

    return CircularIterator<decltype(result)>(result);
  }

  int groupSize(std::optional<Path> group)
  {
    if (!group) {
      return 0;
    }
    if (mGroupContent.find(*group) == mGroupContent.end()) {
      return 0;
    }
    return (int)mGroupContent.at(*group).size();
  }

  std::optional<Thumbnails> getByMedium(std::optional<Path> path)
  {
    if (!path) {
      return std::nullopt;
    }
    if (mSupportByMediumThumbnail.find(*path) ==
        mSupportByMediumThumbnail.end()) {
      return std::nullopt;
    }
    auto index = mSupportByMediumThumbnail.at(*path);
    return mSupport.at(index);
  }

  void addGroup(std::optional<Path> path)
  {
    if (!path) {
      return;
    }
    mGroup.push_back(*path);
    mGroupContent[*path] = std::set<int>();
  }

  void clear()
  {
    mGroupContent.clear();
    mGroup.clear();
    mSupportBySmallThumbnail.clear();
    mSupportByMediumThumbnail.clear();
    mSupportByFullPath.clear();
    mSupport.clear();
  }

  std::vector<Path> const &groups() { return mGroup; }

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