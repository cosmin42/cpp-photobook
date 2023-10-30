#include <pb/PdfPoDoFo.h>

namespace PB {
PdfPoDoFoExport::PdfPoDoFoExport(std::stop_token stopToken,
                                 PaperSettings   paperSettings,
                                 Path            temporaryDirectory)
    : Exportable(stopToken, paperSettings, temporaryDirectory)
{
}

void PdfPoDoFoExport::executeSingleTask()
{
  if (mIndex == mImages.size()) {
    stop();
    return;
  }

  if (!mInitialized) {
    mDocument = std::make_shared<PoDoFo::PdfStreamedDocument>(
        (mDestination / (mName + ".pdf")).string());
    mInitialized = true;
  }

  PoDoFo::PdfPage &pPage = mDocument->GetPages().CreatePage(
      PoDoFo::PdfPage::CreateStandardPageSize(PoDoFo::PdfPageSize::A4));
  auto font = mDocument->GetFonts().SearchFont("Arial");
  PB::basicAssert(font != nullptr);
  auto image = mDocument->CreateImage();
  image->Load(mImages.at(mIndex).string());
  PoDoFo::PdfPainter painter;
  painter.SetCanvas(pPage);
  painter.DrawImage(*image, 0, 0);
  painter.FinishDrawing();

  mIndex++;
}

void PdfPoDoFoExport::finish() {}
} // namespace PB