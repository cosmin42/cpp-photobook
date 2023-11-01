#pragma once
#include <pb/Config.h>

namespace PB {
struct Thumbnails {
  static Thumbnails create();
  static Thumbnails create(Path path);
  static Thumbnails create(Path fPath, Path mPath, Path sPath);

  Path fullPath;
  Path mediumThumbnail;
  Path smallThumbnail = Path(Context::PHOTO_TIMELINE_DEFAULT_IMAGE);
};
} // namespace PB