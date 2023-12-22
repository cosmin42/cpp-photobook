#pragma once

#include <pb/PreprocessedImage.h>
#include <pb/image/VirtualImage.h>

namespace PB {
class RegularImage final : public VirtualImage {
public:
  explicit RegularImage(Thumbnails thumbnails)
  {
    setSizePath(thumbnails.fullPath, thumbnails.mediumThumbnail,
                thumbnails.smallThumbnail);
  }

  RegularImage() = default;
  ~RegularImage() = default;

  VirtualImageType type() const override { return VirtualImageType::Regular; }

  Thumbnails thumbnails() const
  {
    return Thumbnails(resources().full, resources().medium, resources().small);
  }

  void setThumbnail(Thumbnails thumbnails)
  {
    setSizePath(thumbnails.fullPath, thumbnails.mediumThumbnail,
                thumbnails.smallThumbnail);
  }

private:
};
} // namespace PB