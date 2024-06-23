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
  sk_sp<SkData> svgData = read_file(svgPath.string().c_str());

  SkMemoryStream  stream(svgData);
  sk_sp<SkSVGDOM> svgDOM = SkSVGDOM::MakeFromStream(stream);

  if (!svgDOM) {
    fprintf(stderr, "Failed to load SVG file\n");
    return;
  }

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
  // svgDOM->setContainerSize(
  //     SkSize::Make(bounds.width() * scale, bounds.height() * scale));

  // Render the SVG
  // svgDOM->render(canvas);

  // Encode the surface to PNG
  SkPixmap       pixmap;
  sk_sp<SkImage> image = surface->makeImageSnapshot();

  image->peekPixels(&pixmap);

  SkFILEWStream         outFile(outputPath.string().c_str());
  SkPngEncoder::Options options;

  SkPngEncoder::Encode(&outFile, pixmap, options);
}
} // namespace PB
