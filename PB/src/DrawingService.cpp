#include <pb/DrawingService.h>

#include <include/core/SkBitmap.h>
#include <include/core/SkCanvas.h>
#include <include/core/SkStream.h>
#include <include/core/SkSurface.h>
#include <include/encode/SkPngEncoder.h>
#include <include/svg/SkSVGCanvas.h>
#include <modules/svg/include/SkSVGDOM.h>

#include <fstream>

namespace PB {

DrawingService::DrawingService() { SkGraphics::Init(); }

void DrawingService::renderSVG(Path svgPath, Path outputPath,
                               cv::Size imageSize)
{
  UNUSED(svgPath)
  UNUSED(outputPath)
  UNUSED(imageSize)
}
} // namespace PB
