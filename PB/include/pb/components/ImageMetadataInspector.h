#pragma once

#include <exiv2/exiv2.hpp>

#include <pb/infra/Traits.h>

namespace PB {
class ImageMetadataInspector final {
public:
  explicit ImageMetadataInspector(Path path);
  ~ImageMetadataInspector() = default;

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
