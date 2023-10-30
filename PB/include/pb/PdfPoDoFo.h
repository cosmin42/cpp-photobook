#pragma once
#include <pb/Exportable.h>
#include <pb/util/Thread.h>

namespace PB {
class PdfPoDoFoExport final : public Exportable, public Thread {
public:
  void executeSingleTask() override;

  void finish() override;
};
} // namespace PB