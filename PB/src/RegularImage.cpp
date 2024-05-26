#include <pb/image/RegularImage.h>

namespace PB {
RegularImage::RegularImage(Path full, Path medium, Path small, bool processed,
                           std::vector<Path> resourcePath)
    : VirtualImage(ImageResources{full, medium, small})
{
  mProcessed = processed;
  mResourcePath = resourcePath.at(0);
}
} // namespace PB