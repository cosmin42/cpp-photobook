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
  std::shared_ptr<cv::Mat> image = PB::Process::singleColorImage(
      mPaperSettings.width, mPaperSettings.height, {255, 255, 255})();

  auto temporaryImage = ImageReader().read(inputPath);
  PBDev::basicAssert(temporaryImage != nullptr);
  Process::resize({mPaperSettings.width, mPaperSettings.height},
                  true)(temporaryImage);
  PB::Process::overlap(temporaryImage, PB::Process::alignToCenter())(image);

  ImageSetWriter().write(outputPath, image);
}

void PdfExportTask::taskStep()
{
  Path tmpImageDestination = mLocalStatePath / TEMPORARY_PHOTO;
  writeImage(mStagedImages.at(mIndex)->frontend().full, tmpImageDestination);

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
}

} // namespace PB