#pragma once

#include <pb/entities/GenericImage.h>

namespace PB {
class RegularImageV2 final : public GenericImage {
public:
  explicit RegularImageV2(std::string hash, Path original)
      : GenericImage(hash), maybeOriginal(original)
  {
  }

  RegularImageV2(const RegularImageV2 &other)
      : GenericImage(other), maybeOriginal(other.maybeOriginal)
  {
  }

  RegularImageV2(RegularImageV2 &&other) noexcept
      : GenericImage(std::move(other)),
        maybeOriginal(std::move(other.maybeOriginal))
  {
  }

  RegularImageV2 &operator=(const RegularImageV2 &other)
  {
    if (this != &other) {
      GenericImage::operator=(other);
      maybeOriginal = other.maybeOriginal;
    }
    return *this;
  }

  ~RegularImageV2() = default;

  Path original() const { return maybeOriginal; }

  ImageType type() const override { return ImageType::Regular; }

private:
  Path maybeOriginal;
};
} // namespace PB
