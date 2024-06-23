#pragma once

#include <include/core/SkGraphics.h>

#pragma warning(push)
#pragma warning(disable : 5054)
#pragma warning(disable : 4127)
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#pragma warning(pop)

#include <pb/util/Traits.h>

namespace PB {
class DrawingService {
public:
  DrawingService();
  DrawingService(const DrawingService &) = delete;
  DrawingService &operator=(const DrawingService &) = delete;

  ~DrawingService() = default;

  void renderSVG(Path svgPath, Path outputPath, cv::Size imageSize);
};
} // namespace PB
