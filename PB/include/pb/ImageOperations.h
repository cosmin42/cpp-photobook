#pragma once

#pragma warning(push)
#pragma warning(disable : 5054)
#pragma warning(disable : 4127)
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#pragma warning(pop)

#include <pb/util/Traits.h>

namespace PB::Process {
auto resize(cv::Size size, bool keepAspectRatio)
    -> std::function<std::shared_ptr<cv::Mat>(std::shared_ptr<cv::Mat>)>;

auto overlap(cv::Size offset, std::shared_ptr<cv::Mat> source)
    -> std::function<std::shared_ptr<cv::Mat>(std::shared_ptr<cv::Mat>)>;

auto singleColorImage(int32_t width, int32_t height, cv::Scalar color)
    -> std::function<std::shared_ptr<cv::Mat>()>;

auto addText(cv::Size offset, std::string const &text, cv::Scalar color)
    -> std::function<std::shared_ptr<cv::Mat>(std::shared_ptr<cv::Mat>)>;

void readImageWriteThumbnail(Path inputPath, Path outputPath);

void imageWriteThumbnail(std::shared_ptr<cv::Mat> image, Path outputPath);

} // namespace PB::Process