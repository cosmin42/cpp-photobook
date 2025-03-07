#pragma once

#include <pb/components/MapReducer.h>
#include <pb/infra/Traits.h>

namespace PB {
// TODO: Move this to a file that is inherited by the tasks.
class ExportLogicListener {
public:
  virtual void onExportComplete(Path) = 0;
  virtual void onExportAborted(Path) = 0;
  virtual void onExportUpdate(Path) = 0;
};
} // namespace PB