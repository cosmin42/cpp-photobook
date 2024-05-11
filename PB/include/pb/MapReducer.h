#pragma once

#include <functional>
#include <optional>

#include <boost/uuid/uuid.hpp>

namespace PB {
typedef std::pair<boost::uuids::uuid, std::function<void()>>
    IdentifyableFunction;

class MapReducer {
public:
  MapReducer() = default;
  virtual ~MapReducer() = default;

  virtual std::optional<IdentifyableFunction>
  getNext(std::stop_token stopToken) = 0;

  virtual void onFinished(const boost::uuids::uuid id) = 0;
};
} // namespace PB
