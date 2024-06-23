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

void CollageLibraryAssistant::createTemplateThumbnail(Path        templatePath,
                                                      AspectRatio aspectRatio,
                                                      cv::Size    pageSize)
{
  auto imagePaths = createNumberedImages(pageSize);

  BasicSVGModel svgModel = {(unsigned)pageSize.width, (unsigned)pageSize.height,
                            imagePaths};

  auto filename = "template_" + aspectRatio() + "-" +
                  std::to_string(pageSize.width) + "x" +
                  std::to_string(pageSize.height) + ".svg";

  auto thumbnailPath = mCollageLibraryThumbnailsDirectory / filename;

  auto maybeString = mThumbnailsSVGInflater.inflate(svgModel, templatePath);

  if (maybeString) {
    std::ofstream file(thumbnailPath);
    if (file.is_open()) {
      file << *maybeString;
      file.close();
    }
    else {
      PBDev::basicAssert(false);
    }
  }
  else {
    PBDev::basicAssert(false);
  }
}

Path CollageLibraryAssistant::createNumberedImage(cv::Size    pageSize,
                                                  unsigned    index,
                                                  std::string name)
{
  std::shared_ptr<cv::Mat> image = PB::Process::singleColorImage(
      pageSize.width, pageSize.height, {20, 20, 20})();

  Process::CVFontInfo fontInfo;
  fontInfo.color = {255, 255, 255};
  fontInfo.pixelSize = Process::pointsFromPixels(4, 300);
  fontInfo.thickness = 24;

  image = PB::Process::addText({pageSize.width / 2, pageSize.height / 2},
                               std::to_string(index), fontInfo)(image);

  auto imagePath = Path(name);
  bool success = cv::imwrite(imagePath.string(), *image);
  PBDev::basicAssert(success);
  return imagePath;
}
} // namespace PB
