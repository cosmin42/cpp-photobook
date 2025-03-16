#include <pb/infra/FileSupport.h>

namespace PB::infra {
std::shared_ptr<cv::Mat> loadImageToCvMat(Path const path)
{
  auto image = cv::imread(path.string(), cv::IMREAD_COLOR);

  if (image.empty()) {
    return nullptr;
  }

  std::shared_ptr<cv::Mat> inputImage = std::make_shared<cv::Mat>(image);

  if (inputImage == nullptr) {
    return nullptr;
  }

  std::vector<cv::Mat> matChannels;

  cv::split(*inputImage, matChannels);

  if (matChannels.size() == 3) {
    cv::Mat alpha(inputImage->rows, inputImage->cols, CV_8UC1);
    alpha = cv::Scalar(255);
    matChannels.push_back(alpha);
    cv::merge(matChannels, *inputImage);
  }
  else if (matChannels.size() != 4) {
    return nullptr;
  }

  return inputImage;
}
} // namespace PB::infra