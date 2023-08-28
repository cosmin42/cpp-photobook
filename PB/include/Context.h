#pragma once

#include <util/Timer.h>

namespace PB {
class Context final {
public:
  static constexpr Context &inst() { return sContext; }

  static constexpr LogLevel LogLevel() { return LogLevel::Debug; }

private:
  Context() = default;
  Context(Context const &) = delete;
  Context(Context &&) = delete;
  Context &operator=(Context const &) = delete;
  ~Context() = default;

  static Context sContext;
};
} // namespace PB