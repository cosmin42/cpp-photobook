#include <pb/export/PdfLibharu.h>

#include <pb/image/ImageOperations.h>
#include <pb/image/ImageReader.h>
#include <pb/project/Project.h>

namespace PB {

void libharu_error_handler([[maybe_unused]] HPDF_STATUS error_no,
                           [[maybe_unused]] HPDF_STATUS detail_no,
                           [[maybe_unused]] void       *user_data)
{
  PB::printError("Internal error\n");
}

PdfLibharuExportTask::PdfLibharuExportTask(
    Path exportPdfPath, Path localStatePath, PaperSettings paperSettings,
    std::vector<std::shared_ptr<VirtualImage>> const &stagedImages)
    : mPdfPath{exportPdfPath}, mLocalStatePath{localStatePath},
      mPaperSettings{paperSettings}
{
  mPDFFile = HPDF_New(libharu_error_handler, NULL);
  PBDev::basicAssert(mPDFFile != nullptr);
  for (auto const &it : stagedImages) {
    mStagedImages.push_back(it);
  }
}

bool PdfLibharuExportTask::stoppingCondition() const
{
  return mIndex >= mStagedImages.size();
}

int PdfLibharuExportTask::stepsCount() const
{
  return (int)mStagedImages.size();
}

void PdfLibharuExportTask::taskStep()
{
  Path tmpImageDestination = mLocalStatePath / TEMPORARY_PHOTO;

  auto virtualImage = mStagedImages.at(mIndex);

  writeImage(virtualImage->frontend().full, tmpImageDestination);

  HPDF_Page page = HPDF_AddPage(mPDFFile);

  HPDF_Page_SetWidth(page, (HPDF_REAL)mPaperSettings.width);
  HPDF_Page_SetHeight(page, (HPDF_REAL)mPaperSettings.height);

  HPDF_Image image = HPDF_LoadJpegImageFromFile(
      mPDFFile, tmpImageDestination.string().c_str());

  if (!image) {
    HPDF_Free(mPDFFile);
    return;
  }

  HPDF_Page_DrawImage(page, image, 0, 0, (HPDF_REAL)mPaperSettings.width,
                      (HPDF_REAL)mPaperSettings.height);

  mIndex++;

  if (mIndex == stepsCount()) {
    if (HPDF_SaveToFile(mPDFFile, mPdfPath.string().c_str()) != HPDF_OK) {
      HPDF_Free(mPDFFile);
      PBDev::basicAssert(false);
    }

    HPDF_Free(mPDFFile);
  }

#ifdef SIMULATE_SLOW_EXPORTER
  std::this_thread::sleep_for(std::chrono::milliseconds(3000));
#endif
}

std::string PdfLibharuExportTask::name() const
{
  return mPdfPath.filename().string() + "-libharu";
}

std::optional<IdentifyableFunction>
PdfLibharuExportTask::getNext(std::stop_token stopToken)
{
  if (mCrunchedFlag) {
    return std::nullopt;
  }

  mStopToken = stopToken;

  IdentifyableFunction f;
  f.first = RuntimeUUID::newUUID();
  f.second = [this, stopToken{stopToken}]() {
    while (!stoppingCondition() && stopToken.stop_requested()) {
      taskStep();
    }
  };
  mCrunchedFlag = true;
  return f;
}

void PdfLibharuExportTask::onFinished(const boost::uuids::uuid id)
{
  if (mStopToken.stop_requested()) {
    mListener->onExportAborted(id);
  }
  else {
    mListener->onExportComplete(id);
  }
}

void PdfLibharuExportTask::writeImage(Path inputPath, Path outputPath) const
{
  auto imageWidth = mPaperSettings.width * 72 / mPaperSettings.ppi;
  auto imageHeight = mPaperSettings.height * 72 / mPaperSettings.ppi;
  std::shared_ptr<cv::Mat> image =
      PB::Process::singleColorImage(imageWidth, imageHeight, {255, 255, 255})();

  auto temporaryImage = ImageReader().read(inputPath);
  PBDev::basicAssert(temporaryImage != nullptr);
  Process::resize({imageWidth, imageHeight}, true)(temporaryImage);
  PB::Process::overlap(temporaryImage, PB::Process::alignToCenter())(image);

  bool success = cv::imwrite(outputPath.string(), *image);
  PBDev::basicAssert(success);
}
} // namespace PB