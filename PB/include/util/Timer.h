#pragma once

#include <chrono>
#include <common/Log.h>

namespace PB {

template <typename CloseFunction> class Timer final {
public:
  Timer() : mStart(std::chrono::high_resolution_clock::now()) {}
  Timer(Timer const &) = delete;
  Timer(Timer &&) = delete;
  Timer &operator=(Timer const &) = delete;
  ~Timer() { CloseFunction(); }

private:
  std::chrono::time_point<std::chrono::high_resolution_clock> mStart;
};

typedef Timer<TimerPrinter> TimerWithPrint;
} // namespace PB