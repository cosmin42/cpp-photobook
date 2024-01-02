#pragma once

#include <pb/export/Exportable.h>
#include <pb/tasks/Tasks.h>

namespace PB {
class HtmlExporter : public Exportable {
public:
  void executeSingleTask();

  void finish();
};
} // namespace PB