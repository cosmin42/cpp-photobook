#pragma once

#include <pb/export/Exportable.h>

namespace PB {
class JpgExport final : public Exportable {
public:
  void executeSingleTask();

  void finish();
};
} // namespace PB