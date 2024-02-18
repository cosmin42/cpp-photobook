#pragma once

#include <hpdf.h>

#include <pb/image/VirtualImage.h>
#include <pb/util/Traits.h>

namespace PB {

class PdfLibharuExportTask final {
public:
  PdfLibharuExportTask(
      Path exportPdfPath, Path localStatePath, PaperSettings paperSettings,
      std::vector<std::shared_ptr<VirtualImage>> const &stagedImages);
  ~PdfLibharuExportTask() = default;

  bool stoppingCondition() const;
  int  stepsCount() const;
  void taskStep();

  std::string name() const;

private:
  static constexpr const char *TEMPORARY_PHOTO = "temporary-photo.jpg";

  void writeImage(Path inputPath, Path outputPath) const;

  int                                        mIndex = 0;
  std::vector<std::shared_ptr<VirtualImage>> mStagedImages;
  Path                                       mLocalStatePath;
  PaperSettings                              mPaperSettings;
  Path                                       mPdfPath;
  HPDF_Doc                                   mPDFFile;
};
} // namespace PB