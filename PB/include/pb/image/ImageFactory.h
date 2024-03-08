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

private:
  static ImageFactory mFactory;

  std::shared_ptr<Project> mProject;
};
} // namespace PB