#pragma once

#pragma warning(push)
#pragma warning(disable : 5054)
#pragma warning(disable : 4127)
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#pragma warning(pop)

namespace PB::Process {
auto resize(int32_t width, int32_t height)
    -> std::function<void(std::shared_ptr<cv::Mat>)>;

} // namespace PB::Process