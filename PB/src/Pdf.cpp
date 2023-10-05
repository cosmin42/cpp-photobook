#include <pb/Pdf.h>

#include <pb/common/Log.h>

namespace PB {

void error_handler([[maybe_unused]] HPDF_STATUS error_no,
                   [[maybe_unused]] HPDF_STATUS detail_no,
                   [[maybe_unused]] void       *user_data)
{
  PB::printError("Internal error\n");
}
void Pdf::exportImages(Path dest, std::vector<Path> images)
{
  HPDF_Doc pdfFile = HPDF_New(error_handler, NULL);
  if (!pdfFile) {
    PB::printError("Internal error\n");
    return;
  }

  for (auto path : images) {
    HPDF_Page page = HPDF_AddPage(pdfFile);

    HPDF_Page_SetSize(page, HPDF_PAGE_SIZE_A4, HPDF_PAGE_LANDSCAPE);

    HPDF_Image image =
        HPDF_LoadJpegImageFromFile(pdfFile, path.string().c_str());

    if (!image) {
      PB::printError("Internal error\n");
      HPDF_Free(pdfFile);
      return;
    }

    HPDF_Page_DrawImage(page, image, 0, 0, (HPDF_REAL)A4_WIDTH,
                        (HPDF_REAL)A4_HEIGHT);
  }

  if (HPDF_SaveToFile(pdfFile, (dest / "output.pdf").string().c_str()) !=
      HPDF_OK) {
    PB::printError("Internal Error\n");
    HPDF_Free(pdfFile);
    return;
  }

  HPDF_Free(pdfFile);
}
} // namespace PB