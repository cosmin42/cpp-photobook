#include <pb/image/TextImage.h>

namespace PB {
TextImage::TextImage(Path full, Path medium, Path small, bool processed,
        std::vector<Path> resourcePath)
    {
  mProcessed = processed;
  setSizePath(full, medium, small);

  mResourcePath = resourcePath.at(0);
}
} // namespace PB