#pragma once

#include <pb/entities/GenericImage.h>

namespace PB {
class CollageImage : public GenericImage {
public:
  explicit CollageImage(std::string hash, std::vector<Path> sources, std::string name)
      : GenericImage(hash), mSources(sources), mName(name)
  {
  }

  CollageImage(const CollageImage &other)
      : GenericImage(other), mSources(other.mSources)
  {
  }

  CollageImage(CollageImage &&other) noexcept
      : GenericImage(std::move(other)), mSources(std::move(other.mSources))
  {
  }

  CollageImage &operator=(const CollageImage &other)
  {
    if (this != &other) {
      GenericImage::operator=(other);
      mSources = other.mSources;
    }
    return *this;
  }

  ~CollageImage() = default;

  std::vector<Path> sources() const { return mSources; }

  std::string name() const override
  {
    return mName;
  }

  ImageType type() const override { return ImageType::Collage; }

private:
  std::vector<Path> mSources;
  std::string mName;
};
} // namespace PB
