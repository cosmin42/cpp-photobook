#pragma once

#include <pb/image/VirtualImage.h>

namespace PB {
class RegularImage final : public VirtualImage {
public:
  explicit RegularImage(ImageResources defaultResources)
      : VirtualImage(defaultResources)
  {
  }

  explicit RegularImage(ImageResources defaultResources, Path full)
      : VirtualImage(defaultResources), mResourcePath(full)
  {
  }

  explicit RegularImage(Path full, Path medium, Path small, bool processed,
                        std::vector<Path> resourcePath);

  ~RegularImage() = default;

  VirtualImageType type() const override { return VirtualImageType::Regular; }

  std::vector<Path> resources() const override { return {mResourcePath}; }

  Path keyPath() const override { return mResourcePath; }

private:
  Path mResourcePath;
};
} // namespace PB
