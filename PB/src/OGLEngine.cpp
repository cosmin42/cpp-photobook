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
  if (std::holds_alternative<LutImageProcessingData>(imageProcessingData)) {

    auto &diskData = std::get<LutImageProcessingData>(imageProcessingData);

    auto     input = pathToSkImage(diskData.inImage);
    auto     surface = mVulkanManager->getSurface(input->imageInfo());
    SkBitmap bitmap;
    processDiskImage(input, diskData.lut, surface, bitmap);

    writeOutputBitmap(diskData.outImage, bitmap);
  }
  else if (std::holds_alternative<LutInMemoryData>(imageProcessingData)) {
    auto &inMemoryData = std::get<LutInMemoryData>(imageProcessingData);

    auto     input = cvMatToSkImage(inMemoryData.inImage);
    auto     surface = mVulkanManager->getSurface(input->imageInfo());
    SkBitmap bitmap;
    processDiskImage(input, inMemoryData.lut, surface, bitmap);

    skBitmapToCvMat(bitmap).copyTo(inMemoryData.outImage);
  }
  else {
    PBDev::basicAssert(false);
  }
}

void OGLEngine::processDiskImage(sk_sp<SkImage>                image,
                                 std::vector<cv::Vec4f> const &lut,
                                 sk_sp<SkSurface> surface, SkBitmap &output)
{
  // TODO: Recycle the surface and the canvas
  SkCanvas *canvas = surface->getCanvas();

  int lutCubeSize = (int)std::cbrt(lut.size());

  SkImageInfo lutImageInfo = SkImageInfo::Make(
      lutCubeSize, lutCubeSize * lutCubeSize,
      SkColorType::kRGBA_F32_SkColorType, SkAlphaType::kUnpremul_SkAlphaType);

  auto addr = lut.data();

  SkPixmap pixmap(lutImageInfo, addr, lutCubeSize * 4 * sizeof(float));

  output.allocPixels(lutImageInfo);
  bool success =
      output.installPixels(lutImageInfo, (void *)addr,
                           lutCubeSize * 4 * sizeof(float), nullptr, nullptr);
  PBDev::basicAssert(success);

  SkIRect rect = SkIRect::MakeWH(lutCubeSize, lutCubeSize * lutCubeSize);

  SkSurfaceProps        props{};
  sk_sp<SkSpecialImage> lutSpecialImage =
      SkSpecialImages::MakeFromRaster(rect, output, props);

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

  if (!output.tryAllocPixels(image->imageInfo())) {
    PBDev::basicAssert(false);
  }

  if (!surface->readPixels(output, 0, 0)) {
    PBDev::basicAssert(false);
  }
}

void OGLEngine::writeOutputBitmap(Path const &path, SkBitmap &bitmap)
{
  SkFILEWStream outputFile(path.string().c_str());
  if (outputFile.isValid()) {
    SkJpegEncoder::Options options;
    if (!SkJpegEncoder::Encode(&outputFile, bitmap.pixmap(), options)) {
      PBDev::basicAssert(false);
    }
  }
  else {
    PBDev::basicAssert(false);
  }
}

sk_sp<SkImage> OGLEngine::pathToSkImage(Path path)
{
  std::unique_ptr<SkStreamAsset> stream =
      SkStream::MakeFromFile(path.string().c_str());

  if (!stream) {
    spdlog::error("Failed to open input image: {}", path.string());
    PBDev::basicAssert(false);
  }

  return SkImages::DeferredFromEncodedData(stream->getData());
}

cv::Mat OGLEngine::skBitmapToCvMat(SkBitmap const &bitmap)
{
  cv::Mat mat(bitmap.height(), bitmap.width(), CV_8UC4,
              (void *)bitmap.pixmap().addr(), bitmap.pixmap().rowBytes());
  return mat;
}

sk_sp<SkImage> OGLEngine::cvMatToSkImage(cv::Mat const &image)
{
  // Ensure the format is supported by Skia
  int         width = image.cols;
  int         height = image.rows;
  int         channels = image.channels();
  SkColorType colorType = kRGBA_8888_SkColorType;

  if (channels == 1) {
    PBDev::basicAssert(false);
  }
  else if (channels == 3) {
    PBDev::basicAssert(false);
  }
  else if (channels == 4) {
    colorType = kRGBA_8888_SkColorType;
  }
  else {
    PBDev::basicAssert(false);
  }

  // Create SkImage from pixel data
  SkImageInfo imageInfo =
      SkImageInfo::Make(width, height, colorType, kUnpremul_SkAlphaType);
  SkPixmap pixmap(imageInfo, image.data, image.step);

  return SkImages::RasterFromPixmap(pixmap, nullptr, nullptr);
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
  mWorkQueue.enqueue(imageProcessingData);
  mFinishedWorkCondition.wait(lock, [this] { return mFinishedWork; });
}

} // namespace PB::Service
