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

  auto isRotated90Degrees() const -> bool;

private:
  Path                    mPath;
  Exiv2::ExifData         mExifData;
  Exiv2::Image::UniquePtr mImage;
  uint8_t                 mOrientation = 0;
};
} // namespace PB
