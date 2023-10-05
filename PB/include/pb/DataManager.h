#pragma once

#include <ranges>
#include <unordered_map>

#include <pb/MediaMap.h>

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
    return mSupport |
           std::ranges::views::filter([this, root{root}](Thumbnails const &th) {
             if (mGroupContent.find(root) == mGroupContent.end()) {
               return false;
             }
             return mGroupContent.at(root).find(th.index) !=
                    mGroupContent.at(root).end();
           });
  }

private:
  std::unordered_map<Path, std::set<int>> mGroupContent;
  std::vector<Path>                       mGroup;
  std::unordered_map<Path, int>           mSupportBySmallThumbnail;
  std::unordered_map<Path, int>           mSupportByMediumThumbnail;
  std::unordered_map<Path, int>           mSupportByFullPath;
  std::vector<Thumbnails>                 mSupport;
};

class DataManager final {
public:
  std::unordered_map<Path, MediaMap> &mediaData() { return mMediaData; }
  std::vector<Path> &mediaIndexedByType() { return mMediaIndexedByPath; }

  std::unordered_map<Path, Path> const &smallThumbnails(Path root)
  {
    return mThumbnailsSmall.at(root);
  }

  std::unordered_map<Path, Path> const &mediumThumbnails(Path root)
  {
    return mThumbnailsMedium.at(root);
  }

  std::optional<Path> inverseMediumThumbnails(Path path)
  {
    for (auto &[key, value] : mThumbnailsSmallInverse) {
      if (value.find(path) != value.end()) {
        return value.at(path);
      }
    }
    return std::nullopt;
  }

  void newSmallTumbnailMap(Path root)
  {
    mThumbnailsSmall[root];
    mThumbnailsSmallInverse[root];
  }

  void newMediumTumbnailMap(Path root)
  {
    mThumbnailsMedium[root];
    mThumbnailsMediumInverse[root];
  }

  void addSmallThumbnail(Path root, Path fullSize, Path smallSize)
  {
    mThumbnailsSmall[root][fullSize] = smallSize;
    mThumbnailsSmallInverse[root][smallSize] = fullSize;
  }

  void addMediumThumbnail(Path root, Path fullSize, Path mediumSize)
  {
    mThumbnailsMedium[root][fullSize] = mediumSize;
    mThumbnailsMediumInverse[root][mediumSize] = fullSize;
  }

  void clearSmallThumbnails()
  {
    mThumbnailsSmall.clear();
    mThumbnailsSmallInverse.clear();
  }

  void clearMediumThumbnails()
  {
    mThumbnailsMedium.clear();
    mThumbnailsMediumInverse.clear();
  }

  void clear()
  {
    mMediaIndexedByPath.clear();
    mMediaData.clear();
    clearSmallThumbnails();
    clearMediumThumbnails();
  }

  ImageSupport &images() { return mImages; }

private:
  std::unordered_map<Path, MediaMap> mMediaData;
  std::vector<Path>                  mMediaIndexedByPath;

  std::unordered_map<Path, std::unordered_map<Path, Path>> mThumbnailsSmall;
  std::unordered_map<Path, std::unordered_map<Path, Path>>
      mThumbnailsSmallInverse;

  std::unordered_map<Path, std::unordered_map<Path, Path>> mThumbnailsMedium;
  std::unordered_map<Path, std::unordered_map<Path, Path>>
      mThumbnailsMediumInverse;

  ImageSupport mImages;
};
} // namespace PB