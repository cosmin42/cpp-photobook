#pragma once

#include <functional>
#include <optional>
#include <stop_token>

#include <boost/uuid/nil_generator.hpp>
#include <boost/uuid/uuid.hpp>

#include <pb/util/Traits.h>

DECLARE_STRONG_UUID(MapReducerTaskId)

namespace PB {
typedef std::pair<PBDev::MapReducerTaskId, std::function<void()>>
    IdentifyableFunction;

class MapReducer {
public:
  MapReducer() : mId(boost::uuids::nil_uuid()) {}
  virtual ~MapReducer() = default;

  void assignUuid(PBDev::MapReducerTaskId id) { mId = id; }

  bool isFinished() const { return mTaskCount == taskCount(); }

  virtual std::optional<IdentifyableFunction>
  getTask(std::stop_token stopToken) = 0;

  virtual void onTaskFinished(PBDev::MapReducerTaskId) = 0;

  virtual unsigned taskCount() const = 0;

  friend class TaskCruncher;

protected:
  PBDev::MapReducerTaskId mId;

private:
  std::atomic<unsigned> mTaskCount = 0;

  void onTaskFinishedInternal(PBDev::MapReducerTaskId id)
  {
    mTaskCount++;
    onTaskFinished(id);
  }
};
} // namespace PB
