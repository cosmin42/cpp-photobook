#pragma once

#include <pb/VirtualImage.h>

namespace PB {
class TextImage final : public VirtualImage {
public:
  TextImage(std::string text) : mText(text) {}

  VirtualImageType type() const override { return VirtualImageType::Text; }

private:
  std::string mText;
};
} // namespace PB