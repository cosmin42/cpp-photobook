#include <pb/PdfPoDoFo.h>

#include <pb/ImageOperations.h>
#include <pb/ImageReader.h>
#include <pb/ImageSetWriter.h>

namespace PB {
PdfPoDoFoExport::PdfPoDoFoExport(std::stop_token stopToken,
                                 PaperSettings   paperSettings,
                                 Path            temporaryDirectory)
    : Exportable(stopToken, paperSettings, temporaryDirectory)
{
}

void PdfPoDoFoExport::paperProjection(Path inputPath, Path outputPath)
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

void PdfPoDoFoExport::executeSingleTask()
{
  if (!mInitialized) {
    mDocument = std::make_shared<PoDoFo::PdfStreamedDocument>(
        (mDestination / (mName + ".pdf")).string());
    mInitialized = true;
  }

  Path destination = mTemporaryDirectory / TEMPORARY_PHOTO;
  paperProjection(mImages.at(mIndex), destination);

  PoDoFo::PdfPage &pPage = mDocument->GetPages().CreatePage(
      PoDoFo::PdfPage::CreateStandardPageSize(PoDoFo::PdfPageSize::A4, true));
  auto font = mDocument->GetFonts().SearchFont("Arial");
  PBDev::basicAssert(font != nullptr);
  auto image = mDocument->CreateImage();
  image->Load(destination.string());
  PoDoFo::PdfPainter painter;
  painter.SetCanvas(pPage);
  painter.DrawImage(*image, 0, 0);
  painter.FinishDrawing();

  notify();

  mIndex++;
}

void PdfPoDoFoExport::finish() { notify(); }

bool PdfPoDoFoExport::stoppingCondition() { return mIndex >= mImages.size(); }

std::pair<int, int> PdfPoDoFoExport::progress() const { return {mIndex, (int)mImages.size()}; }
} // namespace PB