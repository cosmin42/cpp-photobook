#pragma once

#include <pb/components/MapReducer.h>
#include <pb/infra/Traits.h>

namespace PB {
// TODO: Move this to a file that is inherited by the tasks.
class ExportLogicListener {
public:
  virtual void onExportComplete(PBDev::MapReducerTaskId) = 0;
  virtual void onExportAborted(PBDev::MapReducerTaskId) = 0;
  virtual void onExportUpdate(PBDev::MapReducerTaskId) = 0;
};
} // namespace PB