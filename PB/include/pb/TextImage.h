#pragma once

#include <pb/VirtualImage.h>

namespace PB {
class TextImage final : public VirtualImage {
public:
  TextImage(Thumbnails thumbnails) : VirtualImage(thumbnails) {}

  cv::Mat image() override;
  Path    path() override;
};
} // namespace PB