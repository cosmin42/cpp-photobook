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
  MapReducer() = default;
  virtual ~MapReducer() = default;

  virtual std::optional<IdentifyableFunction> getNext(std::stop_token stopToken)
  {
    UNUSED(stopToken);
#if defined(_MSC_VER) && !defined(__clang__) // MSVC
    __assume(false);
#else // GCC, Clang
    __builtin_unreachable();
#endif
  }

  virtual void onFinished(const boost::uuids::uuid id)
  {
    UNUSED(id);
#if defined(_MSC_VER) && !defined(__clang__) // MSVC
    __assume(false);
#else // GCC, Clang
    __builtin_unreachable();
#endif
  }
};
} // namespace PB
