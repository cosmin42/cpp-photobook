#pragma once

#include <pb/image/VirtualImage.h>

namespace PB {
class TextImage final : public VirtualImage {
public:
  explicit TextImage(Path full) : mResourcePath(full) {}
  explicit TextImage(Path full, Path medium, Path small, bool processed,
                     std::vector<Path> resourcePath);
  ~TextImage() = default;

  VirtualImageType type() const override { return VirtualImageType::Text; }

  std::vector<Path> resources() const override { return {mResourcePath}; }

  Path keyPath() const override { return mResourcePath; }

private:
  Path mResourcePath;
};
} // namespace PB
