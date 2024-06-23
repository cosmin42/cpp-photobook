#include <pb/DrawingService.h>

#include <include/core/SkBitmap.h>
#include <include/core/SkCanvas.h>
#include <include/core/SkStream.h>
#include <include/core/SkSurface.h>
#include <include/encode/SkPngEncoder.h>
#include <include/ports/SkFontMgr_empty.h>
#include <include/svg/SkSVGCanvas.h>
#include <modules/skshaper/utils/FactoryHelpers.h>
#include <modules/svg/include/SkSVGDOM.h>#
#include <modules/svg/include/SkSVGNode.h>
#include <modules/svg/include/SkSVGRenderContext.h>
#include <src/utils/SkOSPath.h>

#include <fstream>

#include <Windows.h>

namespace PB {

sk_sp<SkData> read_file(const char *path)
{
  std::ifstream file(path, std::ios::binary);
  file.seekg(0, std::ios::end);
  size_t size = file.tellg();
  file.seekg(0, std::ios::beg);

  sk_sp<SkData> data = SkData::MakeUninitialized(size);
  file.read((char *)data->writable_data(), size);
  return data;
}

void DrawingService::renderSVG(Path svgPath, Path outputPath,
                               cv::Size imageSize)
{

  svgPath = svgPath.parent_path() / "res" / svgPath.filename();

  auto stream = SkStream::MakeFromFile(svgPath.string().c_str());

  auto dirname = SkOSPath::Dirname(svgPath.string().c_str());

  if (!stream) {
    SkDebugf("Could not open %s.\n", svgPath.string().c_str());
    return;
  }

  auto predecode = skresources::ImageDecodeStrategy::kPreDecode;

  auto x = skresources::FileResourceProvider::Make(
      SkOSPath::Dirname(svgPath.string().c_str()), predecode);

  auto rp = skresources::DataURIResourceProviderProxy::Make(
      skresources::FileResourceProvider::Make(
          SkOSPath::Dirname(svgPath.string().c_str()), predecode),
      predecode, SkFontMgr::RefEmpty());

  SkString assetPath;
  assetPath = SkOSPath::Dirname(svgPath.string().c_str());

  auto svgDOM = SkSVGDOM::Builder()
                    .setFontManager(SkFontMgr::RefEmpty())
                    .setResourceProvider(
                        skresources::FileResourceProvider::Make(assetPath))
                    .setTextShapingFactory(SkShapers::BestAvailable())
                    .make(*stream);

  // Create a surface to render the SVG
  SkImageInfo info =
      SkImageInfo::MakeN32Premul(imageSize.width, imageSize.height);

  auto surface = SkSurfaces::Raster(info);

  if (!surface) {
    fprintf(stderr, "Failed to create surface\n");
    return;
  }

  SkCanvas *canvas = surface->getCanvas();
  canvas->clear(SK_ColorWHITE);

  // Scale the SVG to fit the surface
  SkRect bounds = SkRect::MakeIWH(imageSize.width, imageSize.height);
  float  scaleX = imageSize.width / bounds.width();
  float  scaleY = imageSize.height / bounds.height();
  float  scale = std::min(scaleX, scaleY);

  canvas->scale(scaleX, scaleY);
  svgDOM->setContainerSize(
      SkSize::Make(bounds.width() * scale, bounds.height() * scale));

  // Render the SVG
  svgDOM->render(canvas);

  // Encode the surface to PNG
  SkPixmap       pixmap;
  sk_sp<SkImage> image = surface->makeImageSnapshot();

  image->peekPixels(&pixmap);
  std::vector<std::vector<SkColor>> colors;

  SkFILEWStream         outFile(outputPath.string().c_str());
  SkPngEncoder::Options options;

  SkPngEncoder::Encode(&outFile, pixmap, options);
  outFile.flush();
}
} // namespace PB
