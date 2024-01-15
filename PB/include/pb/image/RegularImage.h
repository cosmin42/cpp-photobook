#pragma once

#include <pb/image/VirtualImage.h>

namespace PB {
class RegularImage final : public VirtualImage {
public:
  explicit RegularImage(Path full) : mResourcePath(full) {}

  RegularImage() = default;
  ~RegularImage() = default;
/*
void configure(Path resourcePath)
{
  mProcessed = false;
  mResourcePath = resourcePath;
}

void configure(Path resourcePath, Path full, Path medium, Path small)
{
  mProcessed = true;
  mResourcePath = resourcePath;
  setImages(full, medium, small);
}
*/
  VirtualImageType type() const override { return VirtualImageType::Regular; }

  std::vector<Path> resources() const override { return {mResourcePath}; }

  Path keyPath() const override { return mResourcePath; }

private:
  Path mResourcePath;
};
} // namespace PB
