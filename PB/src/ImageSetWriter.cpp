#include <ImageSetWriter.h>

#include <opencv2/imgcodecs.hpp>

#include <ConfigDependants.h>

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