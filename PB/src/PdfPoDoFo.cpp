#include <pb/export/PdfPoDoFo.h>

#include <pb/image/Image.h>

namespace PB {
PdfExportTask::PdfExportTask(
    Path exportPdfPath, Path localStatePath, PaperSettings paperSettings,
    std::vector<std::shared_ptr<VirtualImage>> const &stagedImages)
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

  auto temporaryImage = ImageReader().read(inputPath);
  PBDev::basicAssert(temporaryImage != nullptr);
  Process::resize({imageWidth, imageHeight}, true)(temporaryImage);
  PB::Process::overlap(temporaryImage, PB::Process::alignToCenter())(image);

  bool success = cv::imwrite(outputPath.string(), *image);
  PBDev::basicAssert(success);
}

void PdfExportTask::taskStep()
{
  Path tmpImageDestination = mLocalStatePath / TEMPORARY_PHOTO;

  auto virtualImage = mStagedImages.at(mIndex);

  writeImage(virtualImage->frontend().full, tmpImageDestination);

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

  if (mIndex == stepsCount()) {
    mDocument.reset();
  }

  if constexpr (OneConfig::SIMULATE_SLOW_EXPORTER) {
    std::this_thread::sleep_for(std::chrono::milliseconds(3000));
  }
}

std::string PdfExportTask::name() const
{
  return mPdfPath.filename().string() + "podofo";
}
// TODO: implement once somehow
std::optional<IdentifyableFunction>
PdfExportTask::getNext(std::stop_token stopToken)
{
  if (mCrunchedFlag) {
    return std::nullopt;
  }

  mStopToken = stopToken;

  IdentifyableFunction f{
      RuntimeUUID::newUUID(), [this, stopToken{stopToken}, id{f.first}]() {
        // TODO: Find a way to reuse this code in the other exports
        while (!stoppingCondition() && !stopToken.stop_requested()) {
          taskStep();
          mListener->onExportUpdate(mId);
        }
      }};
  mCrunchedFlag = true;
  return f;
}

void PdfExportTask::onFinished(PBDev::MapReducerTaskId id)
{
  if (mStopToken.stop_requested()) {
    mListener->onExportAborted(id);
  }
  else {
    mListener->onExportComplete(id);
  }
}

} // namespace PB