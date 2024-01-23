#pragma once

#include <pb/project/Project.h>
#include <podofo/podofo.h>

namespace PB {

class PdfExportTask final {
public:
  PdfExportTask(Path exportPdfPath, Path localStatePath,
                PaperSettings                                     paperSettings,
                std::vector<std::shared_ptr<VirtualImage>> const &stagedImages);
  ~PdfExportTask() = default;

  bool stoppingCondition() const;
  void taskStep();

private:
  static constexpr const char *TEMPORARY_PHOTO = "temporary-photo.jpg";

  void writeImage(Path inputPath, Path outputPath) const;

  int                                          mIndex = 0;
  std::vector<std::shared_ptr<VirtualImage>>   mStagedImages;
  std::shared_ptr<PoDoFo::PdfStreamedDocument> mDocument = nullptr;
  Path                                         mLocalStatePath;
  PaperSettings                                mPaperSettings;
  Path                                         mPdfPath;
};

} // namespace PB
