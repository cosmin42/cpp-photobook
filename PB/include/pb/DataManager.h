#pragma once

#include <ranges>
#include <unordered_map>

#include <pb/Config.h>
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
  Path smallThumbnail = Path(Context::PHOTO_TIMELINE_DEFAULT_IMAGE);
};

class ImageSupportListener {
public:
  virtual void importFolderAdded(Path root,
                                 CircularIterator<std::vector<Thumbnails>>) = 0;
  virtual void
      stagePhotosUpdated(CircularIterator<std::vector<Thumbnails>>) = 0;
};

class ImageSupport final {
public:
  void setListener(std::shared_ptr<ImageSupportListener> listener);

  void addGroup(std::optional<Path> path);

  void                addFullPaths(Path root, std::vector<Path> const &paths);
  void                addSmallPath(Path fullSize, Path smallSize);
  void                addMediumPath(Path fullSize, Path mediumSize);
  std::vector<Path>   fullPathByGroup(Path group);
  std::optional<Path> groupByIndex(int index);

  void stagePhoto(Thumbnails fullPath, int position = -1);
  void unstagePhoto(std::vector<int> importedFolderIndex);

  void clear();

  auto stagedIterator() -> CircularIterator<std::vector<Thumbnails>>;
  auto unstagedIterator(Path root) -> CircularIterator<std::vector<Thumbnails>>;
  auto unstagedIterator(int importFolderIndex, int index = -1)
      -> CircularIterator<std::vector<Thumbnails>>;

  int                                  groupSize(std::optional<Path> group);
  std::unordered_map<Path, int> const &groups();
  std::vector<Thumbnails>             &stagedPhotos();

private:
  Thumbnails &image(Path fullPath);

  std::unordered_map<Path, int>                 mGroupIndexes;
  std::unordered_map<Path, std::pair<int, int>> mSupportByFullPath;
  std::vector<std::vector<Thumbnails>>          mSupport;
  std::vector<Thumbnails>                       mStagedPhotos;
  std::shared_ptr<ImageSupportListener>         mListener = nullptr;
};

} // namespace PB