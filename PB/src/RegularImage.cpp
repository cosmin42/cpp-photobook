#include <pb/image/RegularImage.h>

namespace PB {
RegularImage::RegularImage(ImageResources imageResources, bool processed,
                           std::vector<Path> resourcePath)
    : VirtualImage(imageResources)
{
  mProcessed = processed;
  mResourcePath = resourcePath.at(0);
}
} // namespace PB