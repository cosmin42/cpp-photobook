#pragma once

#include <functional>

namespace PBDev
{
class ThreadScheduler {
public:
  virtual ~ThreadScheduler() = default;
  virtual void post(std::function<void()> f) = 0;
};
}
