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

  virtual std::optional<IdentifyableFunction>
  getTask(std::stop_token stopToken) = 0;

  virtual void onTaskFinished(PBDev::MapReducerTaskId) = 0;

  virtual unsigned jobSize() const { return 0; }

  virtual bool isInProgress() const { return true; }

protected:
  PBDev::MapReducerTaskId mId;
};
} // namespace PB
