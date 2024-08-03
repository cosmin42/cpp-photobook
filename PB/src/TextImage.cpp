#include <pb/image/TextImage.h>

namespace PB {
TextImage::TextImage(ImageResources imageResources, bool processed,
                     std::vector<Path> resourcePath)
    : VirtualImage(imageResources)
{
  mProcessed = processed;
  mResourcePath = resourcePath.at(0);
}
} // namespace PB