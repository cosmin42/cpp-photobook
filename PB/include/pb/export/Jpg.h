#pragma once

#include <pb/export/Exportable.h>

namespace PB {
class JpgExport final : public Exportable {
public:
  void executeSingleTask() override;

  void finish() override;
};
} // namespace PB