#pragma once

#include <pb/image/VirtualImage.h>

namespace PB {
class TextImage final : public VirtualImage {
public:
  explicit TextImage(std::string text, Path full)
      : mText(text), mResourcePath(full)
  {
  }

  ~TextImage() = default;

  VirtualImageType type() const override { return VirtualImageType::Text; }

  std::vector<Path> resources() const override
  {
    return {mResourcePath};
  }

private:
  std::string mText;
  Path        mResourcePath;
};
} // namespace PB