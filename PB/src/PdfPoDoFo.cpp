#include <pb/export/PdfPoDoFo.h>

#include <pb/infra/FileSupport.h>

namespace PB {
PdfExportTask::PdfExportTask(Path exportPdfPath, Path localStatePath,
                             PaperSettings                       paperSettings,
                             std::vector<GenericImagePtr> const &stagedImages)
    : mPdfPath{exportPdfPath}, mLocalStatePath{localStatePath},
      mPaperSettings{paperSettings}
{
  mDocument =
      std::make_shared<PoDoFo::PdfStreamedDocument>(exportPdfPath.string());
  for (auto const &it : stagedImages) {
    mStagedImages.push_back(it);
  }
}

bool PdfExportTask::stoppingCondition() const
{
  return mIndex >= mStagedImages.size();
}

int PdfExportTask::stepsCount() const { return (int)mStagedImages.size(); }

void PdfExportTask::writeImage(Path inputPath, Path outputPath) const
{
  auto imageWidth = mPaperSettings.width * 72 / mPaperSettings.ppi;
  auto imageHeight = mPaperSettings.height * 72 / mPaperSettings.ppi;
  std::shared_ptr<cv::Mat> image =
      PB::Process::singleColorImage(imageWidth, imageHeight, {255, 255, 255})();

  auto temporaryImage = infra::loadImageToCvMatToFixedSize(
      inputPath, {imageWidth, imageHeight}, Geometry::ScalePolicy::OnlyDown,
      Geometry::OverlapType::Inscribed);

  PBDev::basicAssert(temporaryImage != nullptr);
  Process::resize(temporaryImage, {imageWidth, imageHeight}, true);
  PB::Process::overlap(temporaryImage, PB::Process::alignToCenter())(image);

  bool success = cv::imwrite(outputPath.string(), *image);
  PBDev::basicAssert(success);
}

void PdfExportTask::taskStep()
{
  Path tmpImageDestination = mLocalStatePath / TEMPORARY_PHOTO;

  auto virtualImage = mStagedImages.at(mIndex);
  auto imagePath =
      mPlatformInfo->thumbnailByHash(mProject->first, virtualImage->hash());
  writeImage(imagePath, tmpImageDestination);

  PoDoFo::PdfPage &pPage = mDocument->GetPages().CreatePage(
      PoDoFo::PdfPage::CreateStandardPageSize(PoDoFo::PdfPageSize::A4, true));
  auto font = mDocument->GetFonts().SearchFont("Arial");
  PBDev::basicAssert(font != nullptr);
  auto image = mDocument->CreateImage();
  image->Load(tmpImageDestination.string());
  PoDoFo::PdfPainter painter;
  painter.SetCanvas(pPage);
  painter.DrawImage(*image, 0, 0);
  painter.FinishDrawing();

  mIndex++;

  if constexpr (OneConfig::SIMULATE_SLOW_EXPORTER) {
    std::this_thread::sleep_for(std::chrono::milliseconds(3000));
  }
}

// TODO: implement once somehow, try std::call_once
std::optional<IdentifyableFunction>
PdfExportTask::getTask(std::stop_token stopToken)
{
  if (mCrunchedFlag) {
    return std::nullopt;
  }

  mStopToken = stopToken;

  IdentifyableFunction f{
      RuntimeUUID::newUUID(), [this, stopToken{stopToken}]() {
        // TODO: Find a way to reuse this code in the other exports
        while (!stoppingCondition() && !stopToken.stop_requested()) {
          taskStep();
          mListener->onExportUpdate(mPdfPath);
        }
      }};
  mCrunchedFlag = true;
  return f;
}

void PdfExportTask::onTaskFinished(PBDev::MapReducerTaskId id)
{
  if (mStopToken.stop_requested()) {
    mListener->onExportAborted(mPdfPath);
  }
  else {
    if (mDocument) {
      mDocument.reset();
    }
    mListener->onExportComplete(mPdfPath);
  }
}

} // namespace PB