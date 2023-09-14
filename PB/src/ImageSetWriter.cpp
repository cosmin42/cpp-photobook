#include <pb/ImageSetWriter.h>

#pragma warning(push)
#pragma warning(disable : 5054)
#pragma warning(disable : 4127)
#include <opencv2/imgcodecs.hpp>
#pragma warning(pop)

#include <pb/Config.h>

namespace PB {
void ImageSetWriter::writeImages(std::vector<cv::Mat> const  &image,
                                 std::filesystem::path const &path) const
{
  bool success = cv::imwrite(path.string(), image);
  if (!success) {
    printDebug("File could not be saved to %s", path.string().c_str());
  }
}
} // namespace PB