#include <pb/ImageOperations.h>

namespace PB::Process {
auto resize(int32_t width, int32_t height)
    -> std::function<void(std::shared_ptr<cv::Mat>)>
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
} // namespace PB::Process