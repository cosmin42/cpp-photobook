#include <pb/ImageOperations.h>

#include <pb/ImageReader.h>
#include <pb/ImageSetWriter.h>

namespace PB::Process {
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
    auto     maxRatio = std::max(ratio.width, ratio.height);

    if (maxRatio < 1) {
      return image;
    }
    cv::Size newSize = {(int)floor((image->cols / maxRatio)), (int)floor(image->rows / maxRatio)};

    cv::resize(*image, *image, newSize, 0, 0, cv::INTER_AREA);

    auto newImage = singleColorImage(newSize.width, newSize.height, cv::Scalar{255, 255, 255})();

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

auto overlap(cv::Size offset, std::shared_ptr<cv::Mat> source)
    -> std::function<std::shared_ptr<cv::Mat>(std::shared_ptr<cv::Mat>)>
{
  auto f = [offset{offset}, source{source}](
               std::shared_ptr<cv::Mat> dest) -> std::shared_ptr<cv::Mat> {
    auto [left, top] = offset;

    //cv::Rect roi(left, top, source->cols, source->rows);

    source->copyTo(*dest);

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

void readImageWriteThumbnail(Path inputPath, Path smallOutputPath,
                             Path mediumOutputPath)
{
  auto inputImage = ImageReader().loadImage(inputPath);

  auto mediumImagePointer = PB::Process::resize(
      cv::Size(Context::mediumThumbnailWidth, Context::mediumThumbnailHeight),
      true)(inputImage);
  ImageSetWriter().write(mediumOutputPath, mediumImagePointer);

  auto smallImagePointer = PB::Process::resize(
      cv::Size(Context::thumbnailWidth, Context::thumbnailHeight),
      true)(inputImage);

  ImageSetWriter().write(smallOutputPath, smallImagePointer);
}

void imageWriteThumbnail(std::shared_ptr<cv::Mat> image, Path smallPath,
                         Path mediumPath)
{
  auto mediumImagePointer = PB::Process::resize(
      cv::Size(Context::mediumThumbnailWidth, Context::mediumThumbnailHeight),
      true)(image);

  ImageSetWriter().write(mediumPath, mediumImagePointer);

  auto smallImagePointer = PB::Process::resize(
      cv::Size(Context::thumbnailWidth, Context::thumbnailHeight), true)(image);

  ImageSetWriter().write(smallPath, smallImagePointer);
}

} // namespace PB::Process