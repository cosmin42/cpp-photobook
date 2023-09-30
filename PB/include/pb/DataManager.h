#pragma once

#include <unordered_map>

#include <pb/MediaMap.h>

namespace PB {

class DataManager final {
public:
  std::unordered_map<Path, MediaMap> &mediaData() { return mMediaData; }
  std::vector<Path> &mediaIndexedByType() { return mMediaIndexedByPath; }

  std::unordered_map<Path, Path> const& smallThumbnails(Path root)
  {
    return mThumbnailsSmall.at(root);
  }

  void newSmallTumbnailMap(Path root)
  {
    mThumbnailsSmall[root];
    mThumbnailsSmallInverse[root];
  }

  void addSmallThumbnail(Path root, Path fullSize, Path smallSize)
  {
    mThumbnailsSmall[root][fullSize] = smallSize;
    mThumbnailsSmallInverse[root][smallSize] = fullSize;
  }

  void clearSmallThumbnails()
  {
    mThumbnailsSmall.clear();
    mThumbnailsSmallInverse.clear();
  }

  void clear()
  {
    mMediaIndexedByPath.clear();
    mMediaData.clear();
    clearSmallThumbnails();
  }

private:
  std::unordered_map<Path, MediaMap>                       mMediaData;
  std::vector<Path>                                        mMediaIndexedByPath;
  std::unordered_map<Path, std::unordered_map<Path, Path>> mThumbnailsSmall;
  std::unordered_map<Path, std::unordered_map<Path, Path>>
      mThumbnailsSmallInverse;
};
} // namespace PB