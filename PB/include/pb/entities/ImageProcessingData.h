#pragma once

namespace PB {

struct ImageProcessingData {
  Path inImage;
  Path outImage;

  virtual ~ImageProcessingData() = default;

  virtual ImageProcessingType type() const { return ImageProcessingType::None; }
};
} // namespace PB
