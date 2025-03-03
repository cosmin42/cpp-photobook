#include <pb/components/OGLEngine.h>

#include <fstream>

#pragma warning(push)
#pragma warning(disable : 4244)
#pragma warning(disable : 4267)
#pragma warning(disable : 4100)
#pragma warning(disable : 4996)
#pragma warning(disable : 4201)
#include <include/codec/SkCodec.h>
#include <include/core/SkBitmap.h>
#include <include/core/SkCanvas.h>
#include <include/core/SkImage.h>
#include <include/core/SkImageInfo.h>
#include <include/core/SkStream.h>
#include <include/core/SkSurface.h>
#include <include/encode/SkJpegEncoder.h>
#include <include/gpu/ganesh/SkSurfaceGanesh.h>
#include <src/core/SkSpecialImage.h>
#pragma warning(pop)

#include <pb/entities/LutImageProcessingData.h>
#include <pb/infra/ThreadScheduler.h>
#include <pb/infra/Traits.h>

namespace PB::Service {

void OGLEngine::configurePlatformInfo(
    std::shared_ptr<PlatformInfo> platformInfo)
{
  mPlatformInfo = platformInfo;
}

void OGLEngine::configVulkanManager(
    std::shared_ptr<VulkanManager> vulkanManager)
{
  mVulkanManager = vulkanManager;
}

void OGLEngine::start()
{
  mWorking = true;
  mStopToken = mStopSource.get_token();
  mThread = std::jthread([this] { mainloop(); });
}

void OGLEngine::stop()
{
  if (mWorking) {
    mStopSource.request_stop();
    mWorkQueue.enqueue(LutImageProcessingData());
  }
}

void OGLEngine::loadPrograms()
{
  for (auto const &[name, path] : FRAGMENT_SHADERS_PATHS) {
    auto shaderPath = mPlatformInfo->installationPath / path;
    spdlog::info("Loading shader: {}", shaderPath.string());
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

  mShaderBuilder =
      std::make_shared<SkRuntimeShaderBuilder>(mPrograms.at("lut"));
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

  mWorking = false;
}

void OGLEngine::loadTextureAndRender(
    ImageProcessingData const &imageProcessingData)
{
  if (imageProcessingData.type() == ImageProcessingType::LUT) {

    auto t0 = std::chrono::high_resolution_clock::now();

    auto lutImageProcessingData =
        dynamic_cast<LutImageProcessingData const &>(imageProcessingData);

    std::unique_ptr<SkStreamAsset> stream =
        SkStream::MakeFromFile(lutImageProcessingData.inImage.string().c_str());

    if (!stream) {
      spdlog::error("Failed to open input image: {}",
                    lutImageProcessingData.inImage.string());
      PBDev::basicAssert(false);
    }

    sk_sp<SkImage> image = SkImages::DeferredFromEncodedData(stream->getData());
    if (!image) {
      PBDev::basicAssert(false);
    }

    auto beforeDrawingTime = std::chrono::high_resolution_clock::now();
    sk_sp<SkSurface> surface = VK_NULL_HANDLE;
    auto             gpuContext = mVulkanManager->gpuContext();
    if (gpuContext) {
      surface = SkSurfaces::RenderTarget(gpuContext, skgpu::Budgeted::kNo,
                                         image->imageInfo());
    }

    if (!surface) {
      surface = SkSurfaces::Raster(image->imageInfo());
      if (!surface) {
        PBDev::basicAssert(false);
      }
    }

    auto afterDrawingTime = std::chrono::high_resolution_clock::now();

    // TODO: Recycle the surface and the canvas
    SkCanvas *canvas = surface->getCanvas();

    int lutCubeSize = (int)std::cbrt(lutImageProcessingData.lut.size());

    SkImageInfo lutImageInfo = SkImageInfo::Make(
        lutCubeSize, lutCubeSize * lutCubeSize,
        SkColorType::kRGBA_F32_SkColorType, SkAlphaType::kUnpremul_SkAlphaType);

    auto addr = lutImageProcessingData.lut.data();

    SkPixmap pixmap(lutImageInfo, addr, lutCubeSize * 4 * sizeof(float));

    SkBitmap lutBitmap;
    lutBitmap.allocPixels(lutImageInfo);
    bool success = lutBitmap.installPixels(
        lutImageInfo, addr, lutCubeSize * 4 * sizeof(float), nullptr, nullptr);

    PBDev::basicAssert(success);

    SkIRect rect = SkIRect::MakeWH(lutCubeSize, lutCubeSize * lutCubeSize);

    SkSurfaceProps        props{};
    sk_sp<SkSpecialImage> lutSpecialImage =
        SkSpecialImages::MakeFromRaster(rect, lutBitmap, props);

    sk_sp<SkImage> lutImage = lutSpecialImage->asImage();

    auto imageShader =
        image->makeShader(SkSamplingOptions(SkFilterMode::kLinear));
    auto lutShader =
        lutImage->makeShader(SkSamplingOptions(SkFilterMode::kLinear));

    mShaderBuilder->child("texture1") = imageShader;
    mShaderBuilder->child("lutFlattenedTexture") = lutShader;
    mShaderBuilder->uniform("lutTensorSize") =
        SkV3((float)lutCubeSize, (float)lutCubeSize, (float)lutCubeSize);

    // Apply the shader
    SkPaint paint;
    paint.setShader(mShaderBuilder->makeShader());

    // Draw the image with the shader
    canvas->drawRect(
        SkRect::MakeWH((float)image->width(), (float)image->height()), paint);

    // Save the output to a file
    SkBitmap outputBitmap;
    if (!outputBitmap.tryAllocPixels(image->imageInfo())) {
      PBDev::basicAssert(false);
    }

    if (!surface->readPixels(outputBitmap, 0, 0)) {
      PBDev::basicAssert(false);
    }

    SkFILEWStream outputFile(lutImageProcessingData.outImage.string().c_str());
    if (outputFile.isValid()) {
      SkJpegEncoder::Options options;
      if (!SkJpegEncoder::Encode(&outputFile, outputBitmap.pixmap(), options)) {
        PBDev::basicAssert(false);
      }
    }
    else {
      PBDev::basicAssert(false);
    }

    const std::chrono::duration<double> drawingTime =
        afterDrawingTime - beforeDrawingTime;

    Noir::inst().getLogger()->info("LUT created {}: {}", drawingTime.count(),
                                   lutImageProcessingData.outImage.string());
  }
}

void OGLEngine::applyLut(LutImageProcessingData const &imageProcessingData)
{
  std::unique_lock lock(mWorkMutex);
  mFinishedWork = false;
  mWorkQueue.enqueue(imageProcessingData);
  mFinishedWorkCondition.wait(lock, [this] { return mFinishedWork; });
}

void OGLEngine::applyLutInMemory(LutInMemoryData const &imageProcessingData)
{
  std::unique_lock lock(mWorkMutex);
  mFinishedWork = false;
}

} // namespace PB::Service
