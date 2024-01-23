#pragma once

#include <pb/export/Exportable.h>
#include <pb/project/Project.h>
#include <podofo/podofo.h>

namespace PB {
/*
class PdfExportTask final
{
public:
  PdfExportTask(Path exportPdfPath, Path localStatePath, PaperSettings paperSettings, std::vector<std::shared_ptr<VirtualImage>> const& stagedImages);
  ~PdfExportTask() = default;

bool stoppingCondition() const;
void executeTask();

private:
  static constexpr const char *TEMPORARY_PHOTO = "temporary-photo.jpg";

  void writeImage() const;

  int mIndex = 0;
  std::vector<std::shared_ptr<VirtualImage>> mStagedImages;
  std::shared_ptr<PoDoFo::PdfStreamedDocument> mDocument = nullptr;
  Path mLocalStatePath;
  PaperSettings mPaperSettings;
};
*/
class PdfPoDoFoExport final {
public:
  explicit PdfPoDoFoExport();
  ~PdfPoDoFoExport() = default;
  PdfPoDoFoExport(PdfPoDoFoExport const &other) {}
  PdfPoDoFoExport &operator=(PdfPoDoFoExport const &other) { return *this; }

  void configure(std::shared_ptr<PB::Project> project);
  void configure(std::shared_ptr<PB::PlatformInfo> platformInfo);

  void taskStep();
  bool stoppingCondition();

private:
  static constexpr const char *TEMPORARY_PHOTO = "temporary-photo.jpg";

  void paperProjection(Path input, Path output);

  std::shared_ptr<PoDoFo::PdfStreamedDocument> mDocument = nullptr;
  std::atomic<int>                             mIndex = 0;
  bool                                         mInitialized = false;
};
} // namespace PB
