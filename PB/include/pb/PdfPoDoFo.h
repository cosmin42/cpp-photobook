#pragma once

#include <pb/Exportable.h>
#include <podofo/podofo.h>

namespace PB {
class PdfPoDoFoExport final : public Exportable {
public:
  explicit PdfPoDoFoExport(std::stop_token stopToken);

  void executeSingleTask() override;

  void finish() override;

private:
  std::shared_ptr<PoDoFo::PdfStreamedDocument> mDocument = nullptr;
  int                                          mIndex = 0;
  bool                                         mInitialized = false;
};
} // namespace PB