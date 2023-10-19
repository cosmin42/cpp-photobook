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

typedef std::function<std::pair<int, int>(int, int, int, int)> OffsetFunction;

OffsetFunction alignToCenter();
OffsetFunction defaultAlignment();

bool validExtension(std::optional<Path> path);

auto resize(cv::Size size, bool keepAspectRatio)
    -> std::function<std::shared_ptr<cv::Mat>(std::shared_ptr<cv::Mat>)>;

auto overlap(std::shared_ptr<cv::Mat> source, OffsetFunction offsetFunction)
    -> std::function<std::shared_ptr<cv::Mat>(std::shared_ptr<cv::Mat>)>;

auto singleColorImage(int32_t width, int32_t height, cv::Scalar color)
    -> std::function<std::shared_ptr<cv::Mat>()>;

auto addText(cv::Size offset, std::string const &text, cv::Scalar color)
    -> std::function<std::shared_ptr<cv::Mat>(std::shared_ptr<cv::Mat>)>;

void readImageWriteThumbnail(int width, int height, Path inputPath,
                             Path smallOutputPath, Path mediumOutputPath);

void imageWriteThumbnail(int width, int height, std::shared_ptr<cv::Mat> image,
                         Path smallOutputPath, Path mediumOutputPath);

} // namespace PB::Process