#include <ImageSetWriter.h>

#include <opencv2/imgcodecs.hpp>

#include <ConfigDependants.h>

namespace PB {
void ImageSetWriter::writeImages(std::vector<cv::Mat> const &image,
                                 std::string const          &path) const
{
  bool success = cv::imwrite(path, image);
  if (!success) {
    printDebug("File could not be saved to %s", path.c_str());
  }
}
} // namespace PB