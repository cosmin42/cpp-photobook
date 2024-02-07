#include <pb/image/Image.h>
#include <pb/image/ImageFactory.h>

namespace PB {

ImageFactory ImageFactory::mFactory;

void ImageFactory::configure(std::shared_ptr<Project> project)
{
  mProject = project;
}

std::shared_ptr<RegularImage> ImageFactory::createRegularImage(Path path)
{
  auto regularImage = std::make_shared<RegularImage>(path);
  return regularImage;
}

std::shared_ptr<TextImage>
ImageFactory::createTextImage(Path path, std::string projectName)
{
  std::shared_ptr<cv::Mat> image =
      PB::Process::singleColorImage(3508, 2480, {255, 255, 255})();

  Process::CVFontInfo fontInfo;
  fontInfo.color = {0, 0, 0};
  fontInfo.pixelSize =
      Process::pointsFromPixels(24, mProject->active().paperSettings.ppi);

  image = PB::Process::addText({3508 / 2, 2480 / 2}, path.stem().string(),
                               fontInfo)(image);

  auto imagePath =
      Path() /
      (mProject->active().pathCache.hashCreateIfMissing(path) + ".JPG");

  Process::imageWriteThumbnail(
      image, VirtualImage::platformInfo->localStatePath / imagePath);

  auto textImage = std::make_shared<TextImage>(imagePath);
  return textImage;
}

std::shared_ptr<VirtualImage> ImageFactory::createImage(Path        path,
                                                        std::string projectName)
{
  if (std::filesystem::is_regular_file(path)) {
    return createRegularImage(path);
  }
  else if (std::filesystem::is_directory(path)) {
    return createTextImage(path, projectName);
  }
  else {
    PBDev::basicAssert(false);
    return nullptr;
  }
}

std::shared_ptr<VirtualImage>
ImageFactory::copyImage(std::shared_ptr<VirtualImage> image)
{
  if (image->type() == VirtualImageType::Regular) {
    return std::make_shared<RegularImage>(
        image->frontend().full, image->frontend().medium,
        image->frontend().small, image->processed(), image->resources());
  }
  else if (image->type() == VirtualImageType::Text) {
    return std::make_shared<TextImage>(
        image->frontend().full, image->frontend().medium,
        image->frontend().small, image->processed(), image->resources());
  }
  else {
    PBDev::basicAssert(false);
    return nullptr;
  }
}
} // namespace PB