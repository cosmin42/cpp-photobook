#pragma once

#include <unordered_map>

#include <pb/MediaMap.h>

namespace PB {

class DataManager {
public:
  DataManager() = default;
  ~DataManager() = default;

  std::unordered_map<Path, MediaMap> &mediaData() { return mMediaData; }
  std::vector<Path> &mediaIndexedByType() { return mMediaIndexedByPath; }

  void clear()
  {
    mMediaIndexedByPath.clear();
    mMediaData.clear();
  }

private:
  std::unordered_map<Path, MediaMap> mMediaData;
  std::vector<Path>                  mMediaIndexedByPath;
};
} // namespace PB