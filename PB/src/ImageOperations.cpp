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
} // namespace PB::Process