#pragma once

#include <pb/entities/GenericImage.h>

namespace PB {
class TextImageV2 final : public GenericImage {
public:
  explicit TextImageV2(Path projectPath, std::string hash, std::string text)
      : GenericImage(projectPath, hash), mText(text)
  {
  }
  ~TextImageV2() = default;

  std::string text() const { return mText; }

private:
  std::string mText;
};
} // namespace PB
