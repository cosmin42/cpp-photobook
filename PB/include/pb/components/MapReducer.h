#pragma once

#include <atomic>
#include <functional>
#include <optional>
#include <stop_token>

#include <boost/uuid/nil_generator.hpp>
#include <boost/uuid/uuid.hpp>

#include <pb/infra/Traits.h>
#include <pb/NoirMonitor.h>

DECLARE_STRONG_UUID(MapReducerTaskId)

namespace PB {
typedef std::pair<PBDev::MapReducerTaskId, std::function<void()>>
    IdentifyableFunction;

class MapReducer {
public:
  MapReducer() : mId(boost::uuids::nil_uuid()) {}
  virtual ~MapReducer() = default;

  void assignUuid(PBDev::MapReducerTaskId id) { mId = id; }
  
  void configureNoirMonitor(std::shared_ptr<NoirMonitor> noirMonitor)
  {
    mNoirMonitor = noirMonitor;
  }

  bool isFinished() const
  {
    unsigned currentCount = *mTaskCountPtr;
    auto     count = taskCount();
    return currentCount == count;
  }

  virtual std::optional<IdentifyableFunction>
  getTask(std::stop_token stopToken) = 0;

  virtual void onTaskFinished(PBDev::MapReducerTaskId) = 0;

  virtual unsigned taskCount() const = 0;

  void onTaskFinishedInternal(PBDev::MapReducerTaskId id)
  {
    (*mTaskCountPtr)++;
    onTaskFinished(id);
  }

protected:
  PBDev::MapReducerTaskId mId;

  std::shared_ptr<NoirMonitor> mNoirMonitor = nullptr;

private:
  // TODO: Check why shared_ptr is needed
  std::shared_ptr<std::atomic<unsigned>> mTaskCountPtr =
      std::make_shared<std::atomic<unsigned>>(0);
};
} // namespace PB
