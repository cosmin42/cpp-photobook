#pragma once

#include <pb/entities/GenericImage.h>

namespace PB {
class RegularImageV2 final : public GenericImage {
public:
  explicit RegularImageV2(Path projectPath, std::string hash, Path original)
      : GenericImage(projectPath, hash), maybeOriginal(original)
  {
  }
  ~RegularImageV2() = default;

  std::optional<Path> original() const { return maybeOriginal; }

  ImageType type() const override { return ImageType::Regular; }

private:
  std::optional<Path> maybeOriginal;
};
} // namespace PB
