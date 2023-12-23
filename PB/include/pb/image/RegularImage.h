#pragma once

#include <pb/image/VirtualImage.h>

namespace PB {
class RegularImage final : public VirtualImage {
public:
  explicit RegularImage(Path full, Path medium, Path small)
  {
    setSizePath(full, medium, small);
  }

  explicit RegularImage(Path full)
  {
    setFullSizePath(full);
  }

  RegularImage() = default;
  ~RegularImage() = default;

  VirtualImageType type() const override { return VirtualImageType::Regular; }

private:
};
} // namespace PB