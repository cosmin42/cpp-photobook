#pragma once

#include <pb/export/Exportable.h>
#include <pb/util/SequentialTaskConsumer.h>

namespace PB {
class HtmlExporter : public Exportable {
public:
  void executeSingleTask() override;

  void finish() override;
};
} // namespace PB