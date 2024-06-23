#include <pb/DrawingService.h>

#include <fstream>
#include <memory>

#include <include/core/SkCanvas.h>
#include <include/core/SkImageInfo.h>
#include <include/core/SkSurface.h>
#include <include/encode/SkPngEncoder.h>
#include <modules/skshaper/utils/FactoryHelpers.h>
#include <modules/svg/include/SkSVGDOM.h>

namespace PB {

DrawingService::DrawingService(std::shared_ptr<SkiaResources> resources)
    : mResources(resources)
{
  SkGraphics::Init();
}

void DrawingService::renderToStream(PBDev::SkiaResourcesId resourceId,
                                    SkFILEWStream &outputStream, Path svgPath,
                                    cv::Size originalImageSize)
{
  auto stream = SkStream::MakeFromFile(svgPath.string().c_str());

  auto svgDOM =
      SkSVGDOM::Builder()
          .setFontManager(SkFontMgr::RefEmpty())
          .setResourceProvider(mResources->resourceProvider(resourceId))
          .setTextShapingFactory(SkShapers::BestAvailable())
          .make(*stream);

  // Create a surface to render the SVG
  SkImageInfo info = SkImageInfo::MakeN32Premul(originalImageSize.width,
                                                originalImageSize.height);

  auto surface = SkSurfaces::Raster(info);

  if (!surface) {
    fprintf(stderr, "Failed to create surface\n");
    return;
  }

  SkCanvas *canvas = surface->getCanvas();
  canvas->clear(SK_ColorWHITE);

  // Scale the SVG to fit the surface
  SkRect bounds =
      SkRect::MakeIWH(originalImageSize.width, originalImageSize.height);

  svgDOM->setContainerSize(
      SkSize::Make(originalImageSize.width, originalImageSize.height));

  // Render the SVG
  svgDOM->render(canvas);

  // Encode the surface to PNG
  SkPixmap       pixmap;
  sk_sp<SkImage> image = surface->makeImageSnapshot();

  image->peekPixels(&pixmap);

  SkPngEncoder::Options options;

  SkPngEncoder::Encode(&outputStream, pixmap, options);
  outputStream.flush();
}

void DrawingService::renderToBuffer() {}

} // namespace PB
