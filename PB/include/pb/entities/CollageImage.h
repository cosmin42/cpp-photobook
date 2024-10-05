#pragma once

#include <pb/entities/GenericImage.h>

namespace PB {
class CollageImage : public GenericImage {
public:
  explicit CollageImage(Path projectPath, std::string hash,
                        std::vector<Path> sources)
      : GenericImage(projectPath, hash), mSources(sources)
  {
  }
  ~CollageImage() = default;

  std::vector<Path> sources() const { return mSources; }

  ImageType type() const override { return ImageType::Collage; }

private:
  std::vector<Path> mSources;
};
} // namespace PB
