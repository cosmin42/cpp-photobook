#pragma once

#include <pb/Exportable.h>

namespace PB {
class JpgExport final : public Exportable {
public:
  void executeSingleTask() override;

  void finish() override;
};
} // namespace PB