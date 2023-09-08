#pragma once

#include <functional>

namespace PB {
class Scheduable {
public:
  virtual ~Scheduable() = default;
  virtual void post(std::function<void()>) = 0;
};
} // namespace PB