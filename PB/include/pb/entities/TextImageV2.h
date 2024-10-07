#pragma once

#include <pb/entities/GenericImage.h>

namespace PB {
class TextImageV2 final : public GenericImage {
public:
  explicit TextImageV2(std::string hash, std::string text)
      : GenericImage(hash), mText(text)
  {
  }
  ~TextImageV2() = default;

  std::string text() const { return mText; }

  ImageType type() const override { return ImageType::Text; }

private:
  std::string mText;
};
} // namespace PB
