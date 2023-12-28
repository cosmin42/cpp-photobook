#include <pb/image/ImageFactory.h>

namespace PB {
std::shared_ptr<RegularImage> ImageFactory::createRegularImage(Path path)
{
  auto regularImage = std::make_shared<RegularImage>(path);
  return regularImage;
}

std::shared_ptr<TextImage> ImageFactory::createTextImage(Path path)
{
  auto textImage = std::make_shared<TextImage>(path.stem().string(), path);
  return textImage;
}

std::shared_ptr<VirtualImage> ImageFactory::createImage(Path path)
{
  if (std::filesystem::is_regular_file(path)) {
    return createRegularImage(path);
  }
  else if (std::filesystem::is_directory(path)) {
    return createTextImage(path);
  }
  else {
    PBDev::basicAssert(false);
    return nullptr;
  }
}
} // namespace PB