#pragma once

#include <pb/image/RegularImage.h>
#include <pb/image/TextImage.h>
#include <pb/project/Project.h>

namespace PB {
class ImageFactory final {
public:
  static ImageFactory &inst() { return mFactory; }

  void configure(std::shared_ptr<Project> project);
  void configure(std::shared_ptr<PathCache> pathCache);

  std::shared_ptr<RegularImage> createRegularImage(Path        path,
                                                   std::string projectName);
  std::shared_ptr<TextImage>    createTextImage(Path        path,
                                                std::string projectName);

  std::shared_ptr<VirtualImage> createImage(Path path, std::string projectName);
  std::shared_ptr<VirtualImage> copyImage(std::shared_ptr<VirtualImage> image);

private:
  static ImageFactory mFactory;

  std::shared_ptr<Project> mProject;
  std::shared_ptr<PathCache> mPathCache;
};
} // namespace PB