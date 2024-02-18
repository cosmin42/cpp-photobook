#pragma once

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
  int                                        mIndex = 0;
  std::vector<std::shared_ptr<VirtualImage>> mStagedImages;
  Path                                       mLocalStatePath;
  PaperSettings                              mPaperSettings;
  Path                                       mPdfPath;
};
} // namespace PB