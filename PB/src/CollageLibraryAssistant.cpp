#include <pb/CollageLibraryAssistant.h>

#pragma warning(push)
#pragma warning(disable : 4996)
#include <spdlog/spdlog.h>
#pragma warning(pop)

#include <pb/Config.h>
#include <pb/image/ImageOperations.h>

namespace PB {
CollageLibraryAssistant::CollageLibraryAssistant(Path outputFolder)
    : mThumbnailsSVGInflater(outputFolder), mOutputFolder(outputFolder)
{
}

std::vector<Path>
CollageLibraryAssistant::createNumberedImages(cv::Size pageSize)
{
  std::vector<Path> paths;
  for (auto i = 0; i < OneConfig::NUMBER_OF_COLLAGE_NUMBERED_IMAGES; ++i) {
    auto path = createNumberedImage(
        pageSize, i, "placeholder_" + std::to_string(i) + ".jpg");
    paths.push_back(path);
  }
  return paths;
}

Path CollageLibraryAssistant::createTemplateThumbnail(
    std::vector<Path> numberedImages, Path templatePath,
    AspectRatio aspectRatio, cv::Size pageSize, std::string filename)
{
  BasicSVGModel svgModel = {(unsigned)pageSize.width, (unsigned)pageSize.height,
                            numberedImages};

  // TODO: this is a hack, we need to find a better way to get the name
  std::string originalName = templatePath.stem().string();
  std::replace(originalName.begin(), originalName.end(), '.', '-');

  if (filename.empty()) {
    filename = originalName + "_" + aspectRatio() + "-" +
               std::to_string(pageSize.width) + "x" +
               std::to_string(pageSize.height) + ".svg";
  }

  auto thumbnailPath = mOutputFolder / filename;

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
  return thumbnailPath;
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

  auto imagePath = mOutputFolder / Path(name);
  bool success = cv::imwrite(imagePath.string(), *image);
  PBDev::basicAssert(success);
#ifndef _CLANG_UML_
  spdlog::info("Image created: {}", (mOutputFolder / Path(name)).string());
#endif
  return Path(name);
}
} // namespace PB
