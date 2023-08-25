#include <ImageReader.h>

#include <opencv2/imgcodecs.hpp>

#include <ConfigDependants.h>

namespace PB {
auto ImageReader::read(std::filesystem::path const &path)
    -> std::optional<cv::Mat>
{
  cv::Mat image = cv::imread(path.string(), cv::IMREAD_COLOR);

  if (image.empty()) {
    printDebug("Image %s could not be read", path.string().c_str());
    return std::nullopt;
  }

  return image;
}
} // namespace PB