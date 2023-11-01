#pragma once
#include <pb/Config.h>

namespace PB {
// Turn constructorus into static create methods.
struct Thumbnails {
  Thumbnails() = default;
  explicit Thumbnails(Path path) : fullPath(path) {}
  explicit Thumbnails(Path fPath, Path mPath, Path sPath)
      : fullPath(fPath), mediumThumbnail(mPath), smallThumbnail(sPath)
  {
  }
  Path fullPath;
  Path mediumThumbnail;
  Path smallThumbnail = Path(Context::PHOTO_TIMELINE_DEFAULT_IMAGE);
};
} // namespace PB