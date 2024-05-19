#pragma once

#include <pb/image/RegularImage.h>
#include <pb/image/TextImage.h>
#include <pb/project/Project.h>

namespace PB {
class ImageFactory final {
public:
  static ImageFactory &inst() { return mFactory; }

  void configure(std::shared_ptr<Project> project);

  std::shared_ptr<RegularImage> createRegularImage(Path path);
  std::shared_ptr<TextImage>    createTextImage(Path path, Path hashPath);

  std::shared_ptr<VirtualImage> createImage(Path path, Path hashPath);
  std::shared_ptr<VirtualImage> copyImage(std::shared_ptr<VirtualImage> image);

  // TODO: Fix this if taking into account the platform info...
  std::shared_ptr<VirtualImage> defaultRegularImage()
  {
    if (mDefaultRegularImage == nullptr) {
      mDefaultRegularImage = std::make_shared<RegularImage>();
    }
    return mDefaultRegularImage;
  }

private:
  static ImageFactory mFactory;

  ~ImageFactory() = default;

  std::shared_ptr<Project> mProject;

  std::shared_ptr<VirtualImage> mDefaultRegularImage = nullptr;
};
} // namespace PB