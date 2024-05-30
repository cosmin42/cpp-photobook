#include <pb/CollageLibraryAssistant.h>

#include <pb/image/ImageOperations.h>

namespace PB {
CollageLibraryAssistant::CollageLibraryAssistant(
    Path collageLibraryThumbnailsDirectory, Path sourceTemplateDirectory)
    : mCollageLibraryThumbnailsDirectory(collageLibraryThumbnailsDirectory),
      mThumbnailsSVGInflater(collageLibraryThumbnailsDirectory)
{
}

std::vector<Path>
CollageLibraryAssistant::createNumberedImages(cv::Size pageSize)
{
  std::vector<Path> paths;
  for (auto i = 0; i < MAX_NUMBER_OF_PLACEHOLDER_IMAGES; ++i) {
    auto path = createNumberedImage(
        pageSize, i, "placeholder_" + std::to_string(i) + ".jpg");
    paths.push_back(path);
  }
  return paths;
}

void CollageLibraryAssistant::createTemplateThumbnail(AspectRatio aspectRatio,
                                                      cv::Size    pageSize)
{
  auto imagePaths = createNumberedImages(pageSize);

  BasicSVGModel svgModel = {(unsigned)pageSize.width, (unsigned)pageSize.height,
                            imagePaths};

  auto filename = "template_" + aspectRatio() + "-" +
                  std::to_string(pageSize.width) + "x" +
                  std::to_string(pageSize.height) + ".svg";

  auto thumbnailPath = mCollageLibraryThumbnailsDirectory / filename;

  mThumbnailsSVGInflater.inflate(svgModel, thumbnailPath);
}

Path CollageLibraryAssistant::createNumberedImage(cv::Size    pageSize,
                                                  unsigned    index,
                                                  std::string name)
{
  // TODO: Use index
  UNUSED(index)
  std::shared_ptr<cv::Mat> image = PB::Process::singleColorImage(
      pageSize.width, pageSize.height, {20, 20, 20})();

  auto imagePath = mCollageLibraryThumbnailsDirectory / name;
  bool success = cv::imwrite(imagePath.string(), *image);
  PBDev::basicAssert(success);
  return imagePath;
}
} // namespace PB
