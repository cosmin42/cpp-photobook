#include <pb/ImageOperations.h>

namespace PB::Process {
auto resize(int32_t width, int32_t height)
    -> std::function<std::shared_ptr<cv::Mat>(std::shared_ptr<cv::Mat>)>
{
  auto f = [width{width}, height{height}](
               std::shared_ptr<cv::Mat> image) -> std::shared_ptr<cv::Mat> {
    if (!image) {
      return nullptr;
    }

    cv::resize(*image, *image, cv::Size(width, height), 0, 0, cv::INTER_AREA);

    return image;
  };

  return f;
}

auto overlap(cv::Size offset, std::shared_ptr<cv::Mat> source)
    -> std::function<std::shared_ptr<cv::Mat>(std::shared_ptr<cv::Mat>)>
{
  auto f = [offset{offset}, source{source}](
               std::shared_ptr<cv::Mat> dest) -> std::shared_ptr<cv::Mat> {
    auto [left, top] = offset;

    cv::Rect roi(left, top, source->cols, source->rows);

    source->copyTo((*dest)(roi));

    return dest;
  };

  return f;
}

auto singleColorImage(int32_t width, int32_t height, Vec3i color)
    -> std::function<std::shared_ptr<cv::Mat>()>
{
  auto f = [width{width}, height{height},
            color{color}]() -> std::shared_ptr<cv::Mat> {
    auto &[r, g, b] = color;
    return std::make_shared<cv::Mat>(height, width, CV_8UC4,
                                     cv::Scalar(r, g, b));
  };

  return f;
}

auto addText(cv::Size offset, std::string const &text, cv::Scalar color)
    -> std::function<std::shared_ptr<cv::Mat>(std::shared_ptr<cv::Mat>)>
{
  auto f = [offset{offset}, text{text}, color{color}](
               std::shared_ptr<cv::Mat> image) -> std::shared_ptr<cv::Mat> {
    auto size =
        cv::getTextSize(text, cv::FONT_HERSHEY_COMPLEX_SMALL, 1.0, 1.0, 0);

    cv::putText(*image, text, offset - (size / 2),
                cv::FONT_HERSHEY_COMPLEX_SMALL, 1.0,
                color, 1, cv::LINE_AA);
    return image;
  };

  return f;
}

} // namespace PB::Process