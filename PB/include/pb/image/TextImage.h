#pragma once

#include <pb/image/VirtualImage.h>

namespace PB {
class TextImage final : public VirtualImage {
public:
  explicit TextImage(Path full) : mResourcePath(full) {}

  ~TextImage() = default;

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

  VirtualImageType type() const override { return VirtualImageType::Text; }

  std::vector<Path> resources() const override { return {mResourcePath}; }

  Path keyPath() const override { return mResourcePath; }

private:
  Path mResourcePath;
};
} // namespace PB
