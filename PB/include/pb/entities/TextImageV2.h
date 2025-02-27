#pragma once

#include <pb/entities/GenericImage.h>

namespace PB {
class TextImageV2 final : public GenericImage {
public:
  explicit TextImageV2(std::string hash, std::string text)
      : GenericImage(hash), mText(text)
  {
  }

  TextImageV2(const TextImageV2 &other)
      : GenericImage(other), mText(other.mText)
  {
  }

  TextImageV2(TextImageV2 &&other) noexcept
      : GenericImage(std::move(other)), mText(std::move(other.mText))
  {
  }

  TextImageV2 &operator=(const TextImageV2 &other)
  {
    if (this != &other) {
      GenericImage::operator=(other);
      mText = other.mText;
    }
    return *this;
  }

  ~TextImageV2() = default;

  std::string text() const { return mText; }

  ImageType type() const override { return ImageType::Text; }

private:
  std::string mText;
};
} // namespace PB
