#pragma once

#include <pb/VirtualImage.h>

namespace PB {
class RegularImage final : public VirtualImage {
public:
  RegularImage(Thumbnails thumbnails) : VirtualImage(thumbnails) {}

  cv::Mat image() override;
  Path    path() override;
};
} // namespace PB