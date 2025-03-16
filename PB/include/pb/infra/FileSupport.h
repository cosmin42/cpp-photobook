#pragma once

#include <memory>

#pragma warning(push)
#pragma warning(disable : 5054)
#pragma warning(disable : 4127)
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#pragma warning(pop)

#include <pb/image/ImageOperations.h>
#include <pb/infra/Traits.h>

namespace PB::infra {

/**
 * Only images with 3 and for channels are supported.
 */
std::shared_ptr<cv::Mat> loadImageToCvMat(Path const path);

std::vector<cv::Vec3f> readLutData(Path lutPath);

bool isValidImage(Path const path);

std::shared_ptr<cv::Mat>
loadImageToCvMatToFixedSize(Path const path, cv::Size size,
                            Geometry::ScalePolicy scapePolicy,
                            Geometry::OverlapType scaleType);

} // namespace PB::infra