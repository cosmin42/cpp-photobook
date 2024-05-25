#pragma once

#include <chrono>

namespace PBDev {

class TimerDefaultCloseFunction {
public:
  static void onClose(double){};
};

template <typename CloseFunction = TimerDefaultCloseFunction>
class Timer final {
public:
  Timer() : mStart(std::chrono::system_clock::now()) {}
  Timer(Timer const &) = delete;
  Timer(Timer &&) = delete;
  Timer &operator=(Timer const &) = delete;
  ~Timer() { CloseFunction::onClose(elapsed()); }

  void reset() { mStart = std::chrono::system_clock::now(); }

  double elapsed() const
  {
    const auto                          end = std::chrono::system_clock::now();
    const std::chrono::duration<double> elapsedTime =
        std::chrono::duration_cast<std::chrono::microseconds>(end - mStart);
    return elapsedTime.count();
  }

private:
  std::chrono::time_point<std::chrono::system_clock> mStart;
};

} // namespace PBDev