#pragma once

#include <pb/image/VirtualImage.h>

namespace PB {
class TextImage final : public VirtualImage {
public:
  explicit TextImage(ImageResources defaultImageResources, Path full)
      : VirtualImage(defaultImageResources), mResourcePath(full)
  {
  }

  explicit TextImage(ImageResources imageResources, bool processed,
                     std::vector<Path> resourcePath);
  ~TextImage() = default;

  VirtualImageType type() const override { return VirtualImageType::Text; }

  std::vector<Path> resources() const override { return {mResourcePath}; }

  Path keyPath() const override { return mResourcePath; }

private:
  Path mResourcePath;
};
} // namespace PB
