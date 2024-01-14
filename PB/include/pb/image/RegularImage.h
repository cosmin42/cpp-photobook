#pragma once

#include <pb/image/VirtualImage.h>

namespace PB {
class RegularImage final : public VirtualImage {
public:
  explicit RegularImage(Path full) {}

  RegularImage() = default;
  ~RegularImage() = default;

  VirtualImageType type() const override { return VirtualImageType::Regular; }

  std::vector<Path> resources() const override
  {
    return {mResourcePath};
  }

private:
  Path mResourcePath;
};
} // namespace PB