#include <pb/ImageOperations.h>

#include <pb/ImageReader.h>
#include <pb/ImageSetWriter.h>

namespace PB::Process {
bool validExtension(std::optional<Path> path)
{
  const std::set<std::string> validFileExtensions = {Context::JPG_EXTENSION,
                                                     ".jpeg", ".png"};

  auto extension = path->extension().string();

  std::transform(extension.begin(), extension.end(), extension.begin(),
                 [](unsigned char c) { return std::tolower(c); });

  return (validFileExtensions.contains(extension));
}
auto resize(cv::Size size, bool keepAspectRatio)
    -> std::function<std::shared_ptr<cv::Mat>(std::shared_ptr<cv::Mat>)>
{
  auto fKeppAspectRation =
      [size{size}](std::shared_ptr<cv::Mat> image) -> std::shared_ptr<cv::Mat> {
    if (!image) {
      return nullptr;
    }

    auto &[width, height] = size;

    cv::Size2f ratio = {(float)image->cols / (float)width,
                        (float)image->rows / (float)height};
    auto       maxRatio = std::max(ratio.width, ratio.height);

    if (maxRatio < 1) {
      return image;
    }
    cv::Size newSize = {(int)floor((image->cols / maxRatio)),
                        (int)floor(image->rows / maxRatio)};

    cv::resize(*image, *image, newSize, 0, 0, cv::INTER_AREA);

    auto newImage = singleColorImage(newSize.width, newSize.height,
                                     cv::Scalar{255, 255, 255})();

    image->copyTo(*newImage);

    return image;
  };

  auto f =
      [size{size}](std::shared_ptr<cv::Mat> image) -> std::shared_ptr<cv::Mat> {
    if (!image) {
      return nullptr;
    }

    cv::resize(*image, *image, size, 0, 0, cv::INTER_AREA);

    return image;
  };

  if (keepAspectRatio) {
    return fKeppAspectRation;
  }
  return f;
}

OffsetFunction alignToCenter()
{
  auto f = [](int smallWidth, int smallHeight, int bigWidth,
              int bigHeight) -> std::pair<int, int> {
    auto marginHorizontal = bigWidth - smallWidth;
    auto marginVertical = bigHeight - smallHeight;

    PB::basicAssert(marginHorizontal >= 0);
    PB::basicAssert(marginVertical >= 0);

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

auto addText(cv::Size offset, std::string const &text, cv::Scalar color)
    -> std::function<std::shared_ptr<cv::Mat>(std::shared_ptr<cv::Mat>)>
{
  auto f = [offset{offset}, text{text}, color{color}](
               std::shared_ptr<cv::Mat> image) -> std::shared_ptr<cv::Mat> {
    auto size = cv::getTextSize(text, cv::FONT_HERSHEY_DUPLEX, 1.0, 2, 0);

    cv::putText(*image, text, offset - (size / 2), cv::FONT_HERSHEY_DUPLEX, 1.0,
                color, 2, cv::LINE_AA);
    return image;
  };

  return f;
}

void readImageWriteThumbnail(int screenWidth, int screenHeight, Path inputPath,
                             Path smallOutputPath, Path mediumOutputPath,
                             ThumbnailType thumbnailsType)
{
  if (thumbnailsType == ThumbnailType::None) {
    return;
  }

  auto inputImage = ImageReader().loadImage(inputPath);

  int mediumThumbnailWidth =
      std::max<int>(Context::MEDIUM_THUMBNAIL_WIDTH, screenWidth / 2);
  int mediumThumbnailHeight =
      std::max<int>(Context::MEDIUM_THUMBNAIL_HEIGHT, screenHeight / 2);

  if (thumbnailsType == ThumbnailType::Both ||
      thumbnailsType == ThumbnailType::Medium) {
    auto mediumImagePointer = PB::Process::resize(
        cv::Size(mediumThumbnailWidth, mediumThumbnailHeight),
        true)(inputImage);
    ImageSetWriter().write(mediumOutputPath, mediumImagePointer);
  }

  if (thumbnailsType == ThumbnailType::Both ||
      thumbnailsType == ThumbnailType::Small) {
    auto smallImagePointer =
        PB::Process::resize(cv::Size(Context::SMALL_THUMBNAIL_WIDTH,
                                     Context::SMALL_THUMBNAIL_HEIGHT),
                            true)(inputImage);

    ImageSetWriter().write(smallOutputPath, smallImagePointer);
  }
}

void imageWriteThumbnail(int screenWidth, int screenHeight,
                         std::shared_ptr<cv::Mat> image, Path smallPath,
                         Path mediumPath)
{
  int mediumThumbnailWidth =
      std::max<int>(Context::MEDIUM_THUMBNAIL_WIDTH, screenWidth / 2);
  int mediumThumbnailHeight =
      std::max<int>(Context::MEDIUM_THUMBNAIL_HEIGHT, screenHeight / 2);

  auto mediumImagePointer = PB::Process::resize(
      cv::Size(mediumThumbnailWidth, mediumThumbnailHeight), true)(image);

  ImageSetWriter().write(mediumPath, mediumImagePointer);

  auto smallImagePointer = PB::Process::resize(
      cv::Size(Context::SMALL_THUMBNAIL_WIDTH, Context::SMALL_THUMBNAIL_HEIGHT),
      true)(image);

  ImageSetWriter().write(smallPath, smallImagePointer);
}

} // namespace PB::Process