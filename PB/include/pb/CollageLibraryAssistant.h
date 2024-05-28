#pragma once
#include <pb/util/Traits.h>

namespace PB {
class CollageLibraryAssistant final {
public:
  explicit CollageLibraryAssistant(Path collageLibraryThumbnailsDirectory);
  ~CollageLibraryAssistant() = default;

private:
  Path mCollageLibraryThumbnailsDirectory;
};
} // namespace PB
