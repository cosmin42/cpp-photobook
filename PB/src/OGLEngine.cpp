#include <pb/components/OGLEngine.h>

#include <fstream>

#pragma warning(push)
#pragma warning(disable : 4244)
#pragma warning(disable : 4267)
#include <include/codec/SkCodec.h>
#include <include/core/SkBitmap.h>
#include <include/core/SkCanvas.h>
#include <include/core/SkImage.h>
#include <include/core/SkImageInfo.h>
#include <include/core/SkStream.h>
#include <include/core/SkSurface.h>
#include <include/encode/SkPngEncoder.h>
#include <src/core/SkSpecialImage.h>
#include <src/gpu/graphite/TextureUtils.h>
#pragma warning(pop)

#include <pb/components/ThreadScheduler.h>
#include <pb/entities/LutImageProcessingData.h>
#include <pb/util/Traits.h>

namespace PB::Service {

#ifdef __APPLE__
void OGLEngine::configureThreadScheduler(
    PBDev::ThreadScheduler *threadScheduler)
{
  mThreadScheduler = threadScheduler;
}
#endif
void OGLEngine::configurePlatformInfo(
    std::shared_ptr<PlatformInfo> platformInfo)
{
  mPlatformInfo = platformInfo;
}

void OGLEngine::start(std::stop_token stopToken)
{
  mStopToken = stopToken;
  mThread = std::jthread([this] { mainloop(); });
}

void OGLEngine::stop(std::stop_source stopSource)
{
  stopSource.request_stop();
  mWorkQueue.enqueue(LutImageProcessingData());
}

void OGLEngine::loadPrograms()
{
  for (auto const &[name, path] : FRAGMENT_SHADERS_PATHS) {
    spdlog::info("Loading shader: {}",
                 (mPlatformInfo->installationPath / path).string());
    std::ifstream file(mPlatformInfo->installationPath / path);
    if (!file.is_open()) {
      spdlog::error("Could not open shader file: {}", path.string());
      PBDev::basicAssert(false);
    }
    std::string skslCode((std::istreambuf_iterator<char>(file)),
                         std::istreambuf_iterator<char>());
    auto [effect, error] = SkRuntimeEffect::MakeForShader(SkString(skslCode));
    if (!effect) {
      spdlog::error("Shader compilation failed: {}", error.c_str());
      PBDev::basicAssert(false);
    }
    mPrograms[name] = effect;
  }
}

void OGLEngine::mainloop()
{
  loadPrograms();

  while (!mStopToken.stop_requested()) {
    auto imageProcessingData = mWorkQueue.dequeue();
    loadTextureAndRender(imageProcessingData);

    mFinishedWork = true;
    mFinishedWorkCondition.notify_one();
  }
}

void OGLEngine::loadTextureAndRender(
    ImageProcessingData const &imageProcessingData)
{
  if (imageProcessingData.type() == ImageProcessingType::LUT) {
    auto lutImageProcessingData =
        dynamic_cast<LutImageProcessingData const &>(imageProcessingData);

    std::unique_ptr<SkStreamAsset> stream = SkStream::MakeFromFile(imageProcessingData.inImage.string().c_str());

    if (!stream) {
        spdlog::error("Failed to open input image: {}", imageProcessingData.inImage.string());
        PBDev::basicAssert(false);
    }

    sk_sp<SkImage> image = SkImages::DeferredFromEncodedData(stream->getData());
    if (!image) {
      PBDev::basicAssert(false);
    }

    // Create a surface for drawing
    sk_sp<SkSurface> surface = SkSurfaces::Raster(image->imageInfo());
    if (!surface) {
      PBDev::basicAssert(false);
    }

    // TODO: Recycle the surface and the canvas
    SkCanvas *canvas = surface->getCanvas();

    float lutCubeSize = std::cbrt(lutImageProcessingData.lut.size());

    SkImageInfo lutImageInfo = SkImageInfo::Make(
        lutCubeSize, lutCubeSize * lutCubeSize,
        SkColorType::kRGBA_F32_SkColorType, SkAlphaType::kUnpremul_SkAlphaType);

    auto addr = lutImageProcessingData.lut.data();

    SkPixmap pixmap(lutImageInfo, addr, lutCubeSize * 4 * sizeof(float));

    SkBitmap lutBitmap;
    lutBitmap.allocPixels(lutImageInfo);
    bool success = lutBitmap.installPixels(
        lutImageInfo, addr, lutCubeSize * 4 * sizeof(float), nullptr, nullptr);

    SkIRect rect = SkIRect::MakeWH(lutCubeSize, lutCubeSize * lutCubeSize);

    SkSurfaceProps props{};
    sk_sp<SkSpecialImage> lutSpecialImage =
        SkSpecialImages::MakeFromRaster(rect, lutBitmap, props);

    sk_sp<SkImage> lutImage = lutSpecialImage->asImage();

    auto imageShader =
        image->makeShader(SkSamplingOptions(SkFilterMode::kLinear));
    auto lutShader =
        lutImage->makeShader(SkSamplingOptions(SkFilterMode::kLinear));

    SkRuntimeEffect::ChildPtr children[] = {imageShader, lutShader};

    SkRuntimeShaderBuilder builder(mPrograms.at("lut"));

    builder.child("texture1") = imageShader;
    builder.child("lutFlattenedTexture") = lutShader;
    builder.uniform("lutTensorSize") =
        SkV3(lutCubeSize, lutCubeSize, lutCubeSize);

    // Apply the shader
    SkPaint paint;
    paint.setShader(builder.makeShader());

    // Draw the image with the shader
    canvas->drawRect(SkRect::MakeWH(image->width(), image->height()), paint);

    // Save the output to a file
    SkBitmap outputBitmap;
    if (!outputBitmap.tryAllocPixels(image->imageInfo())) {
      PBDev::basicAssert(false);
    }

    if (!surface->readPixels(outputBitmap, 0, 0)) {
      PBDev::basicAssert(false);
    }

    SkFILEWStream outputFile(imageProcessingData.outImage.string().c_str());
    if (outputFile.isValid()) {
      SkPngEncoder::Options options;
      if (!SkPngEncoder::Encode(&outputFile, outputBitmap.pixmap(), options)) {
        PBDev::basicAssert(false);
      }
    }
    else {
      PBDev::basicAssert(false);
    }
    spdlog::info("LUT created: " + imageProcessingData.outImage.string());
  }
}

void OGLEngine::applyLut(LutImageProcessingData const &imageProcessingData)
{
  std::unique_lock lock(mWorkMutex);
  mFinishedWork = false;
  mWorkQueue.enqueue(imageProcessingData);
  mFinishedWorkCondition.wait(lock, [this] { return mFinishedWork; });
}

} // namespace PB::Service
