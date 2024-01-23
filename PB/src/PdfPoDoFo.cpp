#include <pb/export/PdfPoDoFo.h>

#include <pb/image/Image.h>

namespace PB {
PdfPoDoFoExport::PdfPoDoFoExport()
{
}

void PdfPoDoFoExport::configure(std::stop_token stopToken)
{

}

void PdfPoDoFoExport::configure(std::shared_ptr<PB::Project> project)
{

}

void PdfPoDoFoExport::configure(std::shared_ptr<PB::PlatformInfo> platformInfo)
{

}

void PdfPoDoFoExport::taskStep()
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

  mIndex++;
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

bool PdfPoDoFoExport::stoppingCondition()
{
  return true;
  /* mIndex >= mImages.size();*/
}

} // namespace PB