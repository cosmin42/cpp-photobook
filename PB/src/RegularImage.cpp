#include <pb/image/RegularImage.h>

namespace PB {
RegularImage::RegularImage(Path full, Path medium, Path small, bool processed,
                           std::vector<Path> resourcePath)
{
  mProcessed = processed;
  setSizePath(full, medium, small);

  mResourcePath = resourcePath.at(0);
}
} // namespace PB