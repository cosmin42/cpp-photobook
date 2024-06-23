#pragma once

#pragma warning(push)
#pragma warning(disable : 5054)
#pragma warning(disable : 4127)
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#pragma warning(pop)

#include <include/core/SkGraphics.h>
#include <include/core/SkStream.h>

#include <pb/SkiaResources.h>
#include <pb/util/Traits.h>

namespace PB {
class DrawingService {
public:
  explicit DrawingService(std::shared_ptr<SkiaResources> resources);
  DrawingService(const DrawingService &) = delete;
  DrawingService &operator=(const DrawingService &) = delete;

  ~DrawingService() = default;

  void renderToStream(PBDev::SkiaResourcesId resourceId,
                      SkFILEWStream &fileStream, Path svgPath,
                      cv::Size originalImageSize);

  void renderToBuffer();

private:
  std::shared_ptr<SkiaResources> mResources = nullptr;
};
} // namespace PB
