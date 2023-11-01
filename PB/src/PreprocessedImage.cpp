#include <pb/PreprocessedImage.h>

namespace PB {
Thumbnails Thumbnails::create() { return Thumbnails(); }

Thumbnails Thumbnails::create(Path path)
{
  Thumbnails thumnail;
  thumnail.fullPath = path;
  return thumnail;
}

Thumbnails Thumbnails::create(Path fPath, Path mPath, Path sPath)
{
  Thumbnails thumnail;
  thumnail.fullPath = fPath;
  thumnail.mediumThumbnail = mPath;
  thumnail.smallThumbnail = sPath;
  return thumnail;
}
} // namespace PB