#pragma once

#include <chrono>

namespace PB {

class TimerListener {
public:
  virtual void onStop() = 0;
};

class Timer final {
public:

  Timer() = delete;
  Timer(Timer const &) = delete;
  Timer(Timer &&) = delete;
  Timer &operator=(Timer const &) = delete;
  ~Timer() = default;
};
} // namespace PB