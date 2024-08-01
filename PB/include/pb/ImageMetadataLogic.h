#pragma once

#include <exiv2/exiv2.hpp>

#include <pb/util/Traits.h>

namespace PB {
class ImageMetadataLogic final {
public:
  explicit ImageMetadataLogic(Path path);
  ~ImageMetadataLogic() = default;

  void inspect();

  unsigned width();
  unsigned height();

private:
  Path            mPath;
  Exiv2::ExifData mExifData;
};
} // namespace PB
