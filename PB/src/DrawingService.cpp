#include <pb/DrawingService.h>

#include <fstream>
#include <memory>

namespace PB {

DrawingService::DrawingService(std::shared_ptr<SkiaResources> resources)
    : mResources(resources)
{
  SkGraphics::Init();
}

void DrawingService::renderToStream(SkFILEWStream &fileStream, Path svgPath,
                               cv::Size imageSize)
{
  UNUSED(fileStream)
  UNUSED(svgPath)
  UNUSED(imageSize)
}


void DrawingService::renderToBuffer() {

}

} // namespace PB
