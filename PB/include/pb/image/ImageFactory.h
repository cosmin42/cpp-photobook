#pragma once

#include <pb/image/RegularImage.h>
#include <pb/image/TextImage.h>

namespace PB
{
class ImageFactory
{
public:
  static std::shared_ptr<RegularImage> createRegularImage(Path path);
  static std::shared_ptr<TextImage>    createTextImage(Path path);

  static std::shared_ptr<VirtualImage> createImage(Path path);
};
}