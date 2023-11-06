#pragma once

#include <pb/PreprocessedImage.h>
#include <pb/VirtualImage.h>

namespace PB {
class RegularImage final : public VirtualImage {
public:
  explicit RegularImage(Thumbnails thumbnails)
  {
    setSizePath(thumbnails.fullPath, thumbnails.mediumThumbnail,
                thumbnails.smallThumbnail);
  }

  VirtualImageType type() const override { return VirtualImageType::Regular; }

  Thumbnails thumbnails() const
  {
    return Thumbnails(fullSizePath(), mediumSizePath(), smallSizePath());
  }

  void setThumbnail(Thumbnails thumbnails)
  {
    setSizePath(thumbnails.fullPath, thumbnails.mediumThumbnail,
                thumbnails.smallThumbnail);
  }

private:
};
} // namespace PB