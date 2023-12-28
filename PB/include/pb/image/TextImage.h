#pragma once

#include <pb/image/VirtualImage.h>

namespace PB {
class TextImage final : public VirtualImage {
public:
  explicit TextImage(std::string text, Path full) : mText(text)
  {
    setFullSizePath(full);
  }

  ~TextImage() = default;

  VirtualImageType type() const override { return VirtualImageType::Text; }

private:
  std::string mText;
};
} // namespace PB