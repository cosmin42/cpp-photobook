#include <pb/services/DrawingService.h>

#include <fstream>
#include <memory>

#pragma warning(push)
#pragma warning(disable : 4244)
#pragma warning(disable : 4267)
#include <include/core/SkCanvas.h>
#include <include/core/SkImageInfo.h>
#include <include/encode/SkJpegEncoder.h>
#include <modules/skshaper/utils/FactoryHelpers.h>
#include <modules/svg/include/SkSVGDOM.h>
#pragma warning(pop)

namespace PB::Service {

DrawingService::DrawingService(std::shared_ptr<SkiaResources> resources)
    : mResources(resources)
{
  SkGraphics::Init();
}

void DrawingService::configureVulkanManager(
    std::shared_ptr<VulkanManager> vulkanManager)
{
  mVulkanManager = vulkanManager;
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

  PBDev::basicAssert(svgDOM != nullptr);

  // Create a surface to render the SVG
  SkImageInfo info = SkImageInfo::MakeN32Premul(originalImageSize.width,
                                                originalImageSize.height);

  auto surface = mVulkanManager->getSurface(info);

  if (!surface) {
    fprintf(stderr, "Failed to create surface\n");
    return;
  }

  SkCanvas *canvas = surface->getCanvas();
  canvas->clear(SK_ColorWHITE);

  // Scale the SVG to fit the surface
  SkRect bounds =
      SkRect::MakeIWH(originalImageSize.width, originalImageSize.height);

  UNUSED(bounds);

  svgDOM->setContainerSize(SkSize::Make((SkScalar)originalImageSize.width,
                                        (SkScalar)originalImageSize.height));

  // Render the SVG
  svgDOM->render(canvas);

  // Encode the surface to PNG
  SkPixmap       pixmap;
  sk_sp<SkImage> image = surface->makeImageSnapshot();

  image->peekPixels(&pixmap);

  SkJpegEncoder::Options options;

  auto success = SkJpegEncoder::Encode(&outputStream, pixmap, options);
  PBDev::basicAssert(success);

  outputStream.flush();
}

void DrawingService::renderToBuffer() {}

} // namespace PB::Service
