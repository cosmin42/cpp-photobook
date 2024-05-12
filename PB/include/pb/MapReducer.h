#pragma once

#include <functional>
#include <optional>
#include <stop_token>

#include <boost/uuid/uuid.hpp>

#include <pb/util/Traits.h>

namespace PB {
typedef std::pair<boost::uuids::uuid, std::function<void()>>
    IdentifyableFunction;

class MapReducer {
public:
  MapReducer() {}
  virtual ~MapReducer() = default;

  virtual std::optional<IdentifyableFunction>
  getNext(std::stop_token stopToken) = 0;

  virtual void onFinished(const boost::uuids::uuid id) = 0;
};
} // namespace PB
