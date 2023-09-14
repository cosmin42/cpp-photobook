#include <pb/ImageReader.h>

#pragma warning(push)
#pragma warning(disable : 5054)
#pragma warning(disable : 4127)
#include <opencv2/imgcodecs.hpp>
#pragma warning(pop)

#include <pb/Config.h>

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