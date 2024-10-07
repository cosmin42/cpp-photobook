#pragma once

#include <pb/entities/GenericImage.h>

namespace PB {
class RegularImageV2 final : public GenericImage {
public:
  explicit RegularImageV2(std::string hash, Path original)
      : GenericImage(hash), maybeOriginal(original)
  {
  }
  ~RegularImageV2() = default;

  Path original() const { return maybeOriginal; }

  ImageType type() const override { return ImageType::Regular; }

private:
  Path maybeOriginal;
};
} // namespace PB
