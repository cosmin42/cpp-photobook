#pragma once

#include <unordered_map>

#include <pb/MediaMap.h>

namespace PB {

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

private:
  std::unordered_map<Path, MediaMap> mMediaData;
  std::vector<Path>                  mMediaIndexedByPath;

  std::unordered_map<Path, std::unordered_map<Path, Path>> mThumbnailsSmall;
  std::unordered_map<Path, std::unordered_map<Path, Path>>
      mThumbnailsSmallInverse;

  std::unordered_map<Path, std::unordered_map<Path, Path>> mThumbnailsMedium;
  std::unordered_map<Path, std::unordered_map<Path, Path>>
      mThumbnailsMediumInverse;
};
} // namespace PB