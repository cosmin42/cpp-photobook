#pragma once

#include <pb/image/VirtualImage.h>

namespace PB {
class TextImage final : public VirtualImage {
public:
  explicit TextImage(Path full) :
	  mResourcePath(full) {}

  ~TextImage() = default;

  VirtualImageType type() const override { return VirtualImageType::Text; }

  std::vector<Path> resources() const override { return {mResourcePath}; }

private:
  Path mResourcePath;
};
} // namespace PB