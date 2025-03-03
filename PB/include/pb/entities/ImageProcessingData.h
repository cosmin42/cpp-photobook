#pragma once

namespace PB {

struct ImageProcessingData {
  virtual ~ImageProcessingData() = default;

  virtual ImageProcessingType type() const { return ImageProcessingType::None; }
};
} // namespace PB
