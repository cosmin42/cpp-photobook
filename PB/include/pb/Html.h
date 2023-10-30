#pragma once

#include <pb/Exportable.h>
#include <pb/util/Thread.h>

namespace PB {
class HtmlExporter : public Exportable {
public:
  void executeSingleTask() override;

  void finish() override;
};
} // namespace PB