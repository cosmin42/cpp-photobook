#include <pb/image/ImageOperations.h>

#include <fstream>

#pragma warning(push)
#pragma warning(disable : 4996)
#include <spdlog/spdlog.h>
#pragma warning(pop)

#include <pb/image/ImageReader.h>
#include <pb/infra/FileSupport.h>

namespace PB::Process {
bool validExtension(std::optional<Path> path)
{
  if (!path) {
    return false;
  }
  if (path->filename().string().starts_with('.')) {
    return false;
  }
  const std::set<std::string> validFileExtensions = {OneConfig::JPG_EXTENSION,
                                                     ".jpeg", ".png"};

  auto extension = path->extension().string();

  std::transform(extension.begin(), extension.end(), extension.begin(),
                 [](unsigned char c) { return std::tolower(c); });

  return (validFileExtensions.contains(extension));
}

std::shared_ptr<cv::Mat> resize(std::shared_ptr<cv::Mat> image,
                                cv::Size newSize, bool keepAspectRatio)
{
  if (!image) {
    return nullptr;
  }

  if (!keepAspectRatio) {
    cv::resize(*image, *image, newSize, 0, 0, cv::INTER_AREA);
    return image;
  }

  auto &[width, height] = newSize;

  cv::Size2f ratio = {(float)image->cols / (float)width,
                      (float)image->rows / (float)height};
  auto       maxRatio = std::max(ratio.width, ratio.height);

  if (maxRatio < 1) {
    return image;
  }
  cv::Size finalSize = {(int)floor((image->cols / maxRatio)),
                        (int)floor(image->rows / maxRatio)};

  cv::resize(*image, *image, finalSize, 0, 0, cv::INTER_AREA);

  auto newImage = singleColorImage(finalSize.width, finalSize.height,
                                   cv::Scalar{255, 255, 255})();

  image->copyTo(*newImage);

  return image;
}

std::shared_ptr<cv::Mat> clone(std::shared_ptr<cv::Mat> image)
{
  if (!image) {
    return nullptr;
  }

  return std::make_shared<cv::Mat>(*image);
}

// TODO: improve this function
std::vector<cv::Vec3f> readLutData(Path lutPath)
{
  std::ifstream file(lutPath);
  if (!file.is_open()) {
    throw std::runtime_error("Could not open .cube file");
  }

  std::string            line;
  std::vector<cv::Vec3f> lut;

  struct Counter3d {
    unsigned i = 0;
    unsigned j = 0;
    unsigned k = 0;
    unsigned size = 0;

    Counter3d &operator++()
    {
      if (k < size - 1) {
        ++k;
      }
      else if (j < size - 1) {
        ++j;
        k = 0;
      }
      else if (i < size - 1) {
        ++i;
        j = 0;
        k = 0;
      }
      else {
        PBDev::basicAssert(false);
      }
      return *this;
    }
  };

  Counter3d counter3d;

  // Skip header and comments
  while (std::getline(file, line)) {
    if (line.empty() || line[0] == '#') {
      continue;
    }

    // Read LUT entries
    std::istringstream iss(line);

    if (line.find("LUT_3D_SIZE") != std::string::npos) {
      std::string cubeRtStr;
      iss >> cubeRtStr;
      iss >> cubeRtStr;

      try {
        counter3d.size = std::stoi(cubeRtStr) + 1;
      }
      catch (...) {
        PBDev::basicAssert(false);
      }
      continue;
    }
    if (line.find("DOMAIN_MIN") != std::string::npos) {
      continue;
    }
    if (line.find("DOMAIN_MAX") != std::string::npos) {
      continue;
    }
    if (line.find("TITLE") != std::string::npos) {
      continue;
    }

    PBDev::basicAssert(counter3d.size > 0);

    cv::Vec3f entry;
    if (iss >> entry[0] >> entry[1] >> entry[2]) {
      lut.push_back(entry);
      ++counter3d;
    }
    else {
      PBDev::basicAssert(false);
    }
  }
  file.close();
  return lut;
}

std::shared_ptr<cv::Mat> extractRGBChannels(std::shared_ptr<cv::Mat> image)
{
  if (!image) {
    return nullptr;
  }

  auto cloneImage = clone(image);

  std::vector<cv::Mat> channels;
  cv::split(*cloneImage, channels);

  if (channels.size() == 3) {
    return cloneImage;
  }
  else if (channels.size() == 4) {
    channels.erase(channels.begin() + 3);
    cv::merge(channels, *cloneImage);
  }
  else {
    PBDev::basicAssert(false);
  }

  return cloneImage;
}

std::vector<double> sampleNormalized(unsigned samplePointsCount)
{
  PBDev::basicAssert(samplePointsCount > 1);

  std::vector<double> samples = {0};

  for (unsigned i = 1; i < (unsigned)samplePointsCount - 1; ++i) {
    samples.push_back(static_cast<double>(i) / (samplePointsCount - 1));
  }

  samples.push_back(1);
  return samples;
}

std::shared_ptr<cv::Mat> applySaturation(std::shared_ptr<cv::Mat> image,
                                         double                   saturation)
{
  // Convert input image to HSV color space
  cv::Mat hsvImage;
  cv::cvtColor(*image, hsvImage, cv::COLOR_BGR2HSV);

  // Split the channels
  std::vector<cv::Mat> channels;
  cv::split(hsvImage, channels);

  // Scale the saturation channel
  channels[1] = channels[1] * saturation;
  channels[1] = cv::min(channels[1], 255); // Ensure values stay within range

  // Merge the channels back
  cv::merge(channels, hsvImage);

  auto outputImage = std::make_shared<cv::Mat>(*image);

  cv::cvtColor(hsvImage, *outputImage, cv::COLOR_HSV2BGR);
  return outputImage;
}

void applySaturationInPlace(std::shared_ptr<cv::Mat> image, double saturation)
{
  if (std::abs(saturation - 1.0) <= std::numeric_limits<double>::epsilon()) {
    return;
  }

  // Convert input image to HSV color space
  cv::Mat hsvImage;
  cv::cvtColor(*image, hsvImage, cv::COLOR_BGR2HSV);
  // Split the channels
  std::vector<cv::Mat> channels;
  cv::split(hsvImage, channels);
  // Scale the saturation channel
  channels[1] = channels[1] * saturation;
  channels[1] = cv::min(channels[1], 255); // Ensure values stay within range
  // Merge the channels back
  cv::merge(channels, hsvImage);
  cv::cvtColor(hsvImage, *image, cv::COLOR_HSV2BGR);
}

std::shared_ptr<cv::Mat> applyContrast(std::shared_ptr<cv::Mat> image,
                                       double                   alpha)
{
  auto outputImage = std::make_shared<cv::Mat>(*image);
  image->convertTo(*outputImage, -1, alpha, 0);
  return outputImage;
}

void applyContrastInPlace(std::shared_ptr<cv::Mat> image, double contrast)
{
  if (std::abs(contrast - 1.0) < std::numeric_limits<double>::epsilon()) {
    return;
  }
  image->convertTo(*image, -1, contrast, 0);
}

std::shared_ptr<cv::Mat> applyBrightness(std::shared_ptr<cv::Mat> image,
                                         double                   brightness)
{
  auto outputImage = std::make_shared<cv::Mat>(*image);
  image->convertTo(*outputImage, -1, 1, brightness);
  return outputImage;
}

void applyBrightnessInPlace(std::shared_ptr<cv::Mat> image, double brightness)
{
  if (std::abs(brightness - 0.0) <= std::numeric_limits<double>::epsilon()) {
    return;
  }
  image->convertTo(*image, -1, 1, (brightness * 255));
}

std::shared_ptr<cv::Mat> applyExposure(std::shared_ptr<cv::Mat> image,
                                       double                   exposure)
{
  auto outputImage = std::make_shared<cv::Mat>(*image);
  image->convertTo(*outputImage, -1, pow(2, exposure), 0);
  return outputImage;
}

std::shared_ptr<cv::Mat>
completeWithAlphaChannel(std::shared_ptr<cv::Mat> image)
{
  if (!image) {
    return nullptr;
  }

  auto cloneImage = clone(image);

  std::vector<cv::Mat> channels;
  cv::split(*cloneImage, channels);

  if (channels.size() == 3) {
    cv::Mat alpha(image->rows, image->cols, CV_8UC1);
    alpha = cv::Scalar(255);
    channels.push_back(alpha);
    cv::merge(channels, *cloneImage);
  }
  else if (channels.size() != 4) {
    PBDev::basicAssert(false);
  }

  return cloneImage;
}

void createTextImage(PaperSettings paperSettings, std::string const &text,
                     Path path)
{
  auto blankImage = Process::singleColorImage(
      paperSettings.width, paperSettings.height, {255, 255, 255})();

  Process::CVFontInfo fontInfo;
  fontInfo.color = {0, 0, 0};
  fontInfo.pixelSize = Process::pointsFromPixels(24, paperSettings.ppi);
  fontInfo.thickness = 8;

  auto image =
      PB::Process::addText({paperSettings.width / 2, paperSettings.height / 2},
                           text, fontInfo)(blankImage);

  Process::writeImageOnDisk(image, path);
}

std::shared_ptr<cv::Mat>
ccompleteWithAlphaChannelInPlace(std::shared_ptr<cv::Mat> image)
{
  if (!image) {
    return nullptr;
  }

  std::vector<cv::Mat> channels;
  cv::split(*image, channels);

  if (channels.size() == 3) {
    cv::Mat alpha(image->rows, image->cols, CV_8UC1);
    alpha = cv::Scalar(255);
    channels.push_back(alpha);
    cv::merge(channels, *image);
  }
  else if (channels.size() != 4) {
    PBDev::basicAssert(false);
  }

  return image;
}

OffsetFunction alignToCenter()
{
  auto f = [](int smallWidth, int smallHeight, int bigWidth,
              int bigHeight) -> std::pair<int, int> {
    auto marginHorizontal = bigWidth - smallWidth;
    auto marginVertical = bigHeight - smallHeight;

    PBDev::basicAssert(marginHorizontal >= 0);
    PBDev::basicAssert(marginVertical >= 0);

    auto marginLeft = marginHorizontal / 2;
    auto marginTop = marginVertical / 2;

    return {marginLeft, marginTop};
  };
  return f;
}

OffsetFunction defaultAlignment()
{
  auto f = []([[maybe_unused]] int smallWidth, [[maybe_unused]] int smallHeight,
              [[maybe_unused]] int bigWidth,
              [[maybe_unused]] int bigHeight) -> std::pair<int, int> {
    return {0, 0};
  };
  return f;
}

auto overlap(std::shared_ptr<cv::Mat> source, OffsetFunction offsetFunction)
    -> std::function<std::shared_ptr<cv::Mat>(std::shared_ptr<cv::Mat>)>
{
  auto f = [source{source}, offsetFunction](
               std::shared_ptr<cv::Mat> dest) -> std::shared_ptr<cv::Mat> {
    auto bigWidth = dest->cols;
    auto bigHeight = dest->rows;

    auto smallWidth = source->cols;
    auto smallHeight = source->rows;

    auto [marginLeft, marginTop] =
        offsetFunction(smallWidth, smallHeight, bigWidth, bigHeight);

    source->copyTo(dest->operator()(
        cv::Rect(marginLeft, marginTop, source->cols, source->rows)));

    return dest;
  };

  return f;
}

auto singleColorImage(int32_t width, int32_t height, cv::Scalar color)
    -> std::function<std::shared_ptr<cv::Mat>()>
{
  auto f = [width{width}, height{height},
            color{color}]() -> std::shared_ptr<cv::Mat> {
    return std::make_shared<cv::Mat>(height, width, CV_8UC4, color);
  };

  return f;
}

auto addText(cv::Size offset, std::string const &text, CVFontInfo fontInfo)
    -> std::function<std::shared_ptr<cv::Mat>(std::shared_ptr<cv::Mat>)>
{
  auto f = [offset{offset}, text{text}, fontInfo{fontInfo}](
               std::shared_ptr<cv::Mat> image) -> std::shared_ptr<cv::Mat> {
    auto size = cv::getTextSize(text, cv::FONT_HERSHEY_DUPLEX,
                                fontInfo.pixelSize, fontInfo.thickness, 0);
    auto totalOffset = cv::Size{offset.width - (size.width / 2),
                                offset.height + (size.height / 2)};
    cv::putText(*image, text, totalOffset, cv::FONT_HERSHEY_DUPLEX,
                fontInfo.pixelSize, fontInfo.color, fontInfo.thickness,
                cv::LINE_AA);
    return image;
  };

  return f;
}

void readImageWriteThumbnail(int screenWidth, int screenHeight, Path inputPath,
                             Path medium, Path small,
                             ThumbnailType thumbnailsType)
{
  if (thumbnailsType == ThumbnailType::None) {
    return;
  }

  auto inputImage = infra::loadImageToCvMat(inputPath);

  int mediumThumbnailWidth =
      std::max<int>(OneConfig::MEDIUM_THUMBNAIL_WIDTH, screenWidth / 2);
  int mediumThumbnailHeight =
      std::max<int>(OneConfig::MEDIUM_THUMBNAIL_HEIGHT, screenHeight / 2);

  if ((thumbnailsType & ThumbnailType::Medium) == ThumbnailType::Medium) {
    auto mediumImagePointer = PB::Process::resize(
        inputImage, {mediumThumbnailWidth, mediumThumbnailHeight}, true);
    bool success = cv::imwrite(medium.string(), *mediumImagePointer);
    PBDev::basicAssert(success);
  }

  if ((thumbnailsType & ThumbnailType::Small) == ThumbnailType::Small) {
    auto smallImagePointer = PB::Process::resize(
        inputImage,
        {OneConfig::SMALL_THUMBNAIL_WIDTH, OneConfig::SMALL_THUMBNAIL_HEIGHT},
        true);

    bool success = cv::imwrite(small.string(), *smallImagePointer);
    PBDev::basicAssert(success);
  }
}

void writeImageOnDisk(std::shared_ptr<cv::Mat> image, Path full)
{
  bool success = cv::imwrite(full.string(), *image);
  PBDev::basicAssert(success);
#ifndef _CLANG_UML_
  spdlog::info("Image written to {}", full.string());
#endif
}

void imageWriteThumbnail(int screenWidth, int screenHeight,
                         std::shared_ptr<cv::Mat> image, Path medium,
                         Path small)
{
  int mediumThumbnailWidth =
      std::max<int>(OneConfig::MEDIUM_THUMBNAIL_WIDTH, screenWidth / 2);
  int mediumThumbnailHeight =
      std::max<int>(OneConfig::MEDIUM_THUMBNAIL_HEIGHT, screenHeight / 2);

  auto mediumImagePointer = PB::Process::resize(
      image, {mediumThumbnailWidth, mediumThumbnailHeight}, true);

  bool success = cv::imwrite(medium.string(), *mediumImagePointer);
  PBDev::basicAssert(success);
#ifndef _CLANG_UML_
  spdlog::info("Medium thumbnail written to {}", medium.string());
#endif

  auto smallImagePointer = PB::Process::resize(
      image,
      {OneConfig::SMALL_THUMBNAIL_WIDTH, OneConfig::SMALL_THUMBNAIL_HEIGHT},
      true);

  success = cv::imwrite(small.string(), *smallImagePointer);
  PBDev::basicAssert(success);
#ifndef _CLANG_UML_
  spdlog::info("Small thumbnail written to {}", small.string());
#endif
}

unsigned pointsFromPixels(double points, unsigned ppi)
{
  return (unsigned)(points * ppi) / 72;
}

} // namespace PB::Process

namespace PB::Geometry {

std::tuple<cv::Point2i, cv::Point2i, cv::Size> overlapCenter(cv::Size src,
                                                             cv::Size dst)
{
  int offsetW = (dst.width - src.width) / 2;
  int offsetH = (dst.height - src.height) / 2;

  cv::Point2i srcOrigin = {0, 0};
  cv::Point2i dstOrigin = {0, 0};

  dstOrigin.x = std::max<int>(0, offsetW);
  dstOrigin.y = std::max<int>(0, offsetH);

  srcOrigin.x = std::abs(std::min<int>(0, offsetW));
  srcOrigin.y = std::abs(std::min<int>(0, offsetH));

  cv::Size sizeCopied;

  sizeCopied.width = std::min<int>(src.width, dst.width);
  sizeCopied.height = std::min<int>(src.height, dst.height);

  return {srcOrigin, dstOrigin, sizeCopied};
}

cv::Size2d computeRatio(cv::Size original, cv::Size reference)
{
  auto rw = (double)original.width / (double)reference.width;
  auto rh = (double)original.height / (double)reference.height;
  return {rw, rh};
}

bool checkPolicy(double ratio, ScalePolicy scalePolicy)
{
  switch (scalePolicy) {
  case ScalePolicy::OnlyDown:
    return ratio >= 1.0;
  case ScalePolicy::OnlyUp:
    return ratio <= 1.0;
  case ScalePolicy::Both:
    return true;
  }
  return true;
}

cv::Size resizeBox(cv::Size original, cv::Size boundingBox,
                   OverlapType overlapType, ScalePolicy scalePolicy)
{
  auto ratio = computeRatio(original, boundingBox);

  double circumscribedRatio = 0.0;
  if (overlapType == OverlapType::Circumscribed) {
    circumscribedRatio = std::min<double>(ratio.width, ratio.height);
  }
  else if (overlapType == OverlapType::Inscribed) {
    circumscribedRatio = std::max<double>(ratio.width, ratio.height);
  }
  else {
    PBDev::basicAssert(false);
  }

  if (!checkPolicy(circumscribedRatio, scalePolicy)) {
    return original;
  }

  cv::Size result = {
      static_cast<int>((double)original.width / circumscribedRatio),
      static_cast<int>((double)original.height / circumscribedRatio)};
  return result;
}
std::tuple<cv::Size, cv::Size, cv::Size>
compute3SizesGeometry(cv::Size originalSize, cv::Size paperSize)
{
  auto smallSize = PB::Geometry::resizeBox(
      originalSize,
      cv::Size{OneConfig::SMALL_THUMBNAIL_WIDTH,
               OneConfig::SMALL_THUMBNAIL_HEIGHT},
      Geometry::OverlapType::Inscribed, Geometry::ScalePolicy::OnlyDown);

  auto mediumSize = PB::Geometry::resizeBox(
      originalSize,
      cv::Size{OneConfig::MEDIUM_THUMBNAIL_WIDTH,
               OneConfig::MEDIUM_THUMBNAIL_HEIGHT},
      Geometry::OverlapType::Inscribed, Geometry::ScalePolicy::OnlyDown);

  auto largeSize = PB::Geometry::resizeBox(
      originalSize, cv::Size{paperSize.width, paperSize.height},
      Geometry::OverlapType::Circumscribed, Geometry::ScalePolicy::OnlyDown);

  return {largeSize, mediumSize, smallSize};
}
} // namespace PB::Geometry
