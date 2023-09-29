#pragma once

#include <unordered_map>

#include <pb/MediaMap.h>

namespace PB {

class DataManager final {
public:
  std::unordered_map<Path, MediaMap> &mediaData() { return mMediaData; }
  std::vector<Path> &mediaIndexedByType() { return mMediaIndexedByPath; }
  std::unordered_map<Path, Path> &smallThumbnails() { return mThumbnailsSmall; }

  void clear()
  {
    mMediaIndexedByPath.clear();
    mMediaData.clear();
    mThumbnailsSmall.clear();
  }

private:
  std::unordered_map<Path, MediaMap> mMediaData;
  std::vector<Path>                  mMediaIndexedByPath;
  std::unordered_map<Path, Path>     mThumbnailsSmall;
};
} // namespace PB