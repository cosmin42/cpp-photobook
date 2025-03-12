#pragma once

#include <optional>

#pragma warning(push)
#pragma warning(disable : 5054)
#pragma warning(disable : 4127)
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#pragma warning(pop)

#include <pb/Enums.h>
#include <pb/entities/PaperSettings.h>
#include <pb/infra/Traits.h>

namespace PB::Process {

struct CVFontInfo {
  cv::Scalar color;
  unsigned   pixelSize;
  unsigned   thickness;
};

typedef std::function<std::pair<int, int>(int, int, int, int)> OffsetFunction;

OffsetFunction alignToCenter();
OffsetFunction defaultAlignment();

bool validExtension(std::optional<Path> path);

std::shared_ptr<cv::Mat> resize(std::shared_ptr<cv::Mat> image,
                                cv::Size newSize, bool keepAspectRatio);

std::shared_ptr<cv::Mat> clone(std::shared_ptr<cv::Mat> image);

std::vector<cv::Vec3f> readLutData(Path lutPath);

std::shared_ptr<cv::Mat> extractRGBChannels(std::shared_ptr<cv::Mat> image);

std::vector<double> sampleNormalized(unsigned samplePointsCount);

std::shared_ptr<cv::Mat> applySaturation(std::shared_ptr<cv::Mat> image,
                                         double                   saturation);

void applySaturationInPlace(std::shared_ptr<cv::Mat> image, double saturation);

std::shared_ptr<cv::Mat> applyContrast(std::shared_ptr<cv::Mat> image,
                                       double                   contrast);

void applyContrastInPlace(std::shared_ptr<cv::Mat> image, double contrast);

std::shared_ptr<cv::Mat> applyBrightness(std::shared_ptr<cv::Mat> image,
                                         double                   brightness);

void applyBrightnessInPlace(std::shared_ptr<cv::Mat> image, double brightness);

std::shared_ptr<cv::Mat> applyExposure(std::shared_ptr<cv::Mat> image,
                                       double                   exposure);

std::shared_ptr<cv::Mat>
completeWithAlphaChannel(std::shared_ptr<cv::Mat> image);

void createTextImage(PaperSettings paperSettings, std::string const &text,
                     Path path);

// TODO: Refactor all the studpid functions like resize
auto overlap(std::shared_ptr<cv::Mat> source, OffsetFunction offsetFunction)
    -> std::function<std::shared_ptr<cv::Mat>(std::shared_ptr<cv::Mat>)>;

auto singleColorImage(int32_t width, int32_t height, cv::Scalar color)
    -> std::function<std::shared_ptr<cv::Mat>()>;

auto addText(cv::Size offset, std::string const &text, CVFontInfo fontInfo)
    -> std::function<std::shared_ptr<cv::Mat>(std::shared_ptr<cv::Mat>)>;

void readImageWriteThumbnail(int width, int height, Path full, Path medium,
                             Path small, ThumbnailType thumbnailType);

void writeImageOnDisk(std::shared_ptr<cv::Mat> image, Path full);

void imageWriteThumbnail(int width, int height, std::shared_ptr<cv::Mat> image,
                         Path medium, Path small);

unsigned pointsFromPixels(double points, unsigned ppi);

} // namespace PB::Process

namespace PB::Geometry {
enum class OverlapType { Circumscribed, Inscribed };
enum class ScalePolicy { OnlyUp, OnlyDown, Both };

std::tuple<cv::Point2i, cv::Point2i, cv::Size> overlapCenter(cv::Size src,
                                                             cv::Size dst);

cv::Size2d computeRatio(cv::Size original, cv::Size reference);

bool checkPolicy(double ratio, ScalePolicy scalePolicy);

cv::Size resizeBox(cv::Size original, cv::Size boundingBox,
                   OverlapType overlapType, ScalePolicy scalePolicy);

std::tuple<cv::Size, cv::Size, cv::Size>
compute3SizesGeometry(cv::Size originalSize, cv::Size paperSize);

} // namespace PB::Geometry