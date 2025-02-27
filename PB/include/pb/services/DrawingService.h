#pragma once

#pragma warning(push)
#pragma warning(disable : 5054)
#pragma warning(disable : 4127)
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#pragma warning(pop)

#pragma warning(push)
#pragma warning(disable : 4244)
#pragma warning(disable : 4267)
#include <include/core/SkGraphics.h>
#include <include/core/SkStream.h>
#pragma warning(pop)

#include <pb/components/SkiaResources.h>
#include <pb/infra/Traits.h>

namespace PB::Service {
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
