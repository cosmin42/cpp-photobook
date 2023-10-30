#pragma once

#include <pb/Exportable.h>
#include <podofo/podofo.h>

namespace PB {
class PdfPoDoFoExport final : public Exportable {
public:
  explicit PdfPoDoFoExport(std::stop_token stopToken,
                           PaperSettings   paperSettings,
                           Path            temporaryDirectory);

  void executeSingleTask() override;

  void finish() override;

private:
  static constexpr const char *TEMPORARY_PHOTO = "temporary-photo.jpg";

  void paperProjection(Path input, Path output);

  std::shared_ptr<PoDoFo::PdfStreamedDocument> mDocument = nullptr;
  int                                          mIndex = 0;
  bool                                         mInitialized = false;
};
} // namespace PB