#pragma once

#include <winrt/base.h>

namespace NativePB {
class Converter final {
public:
  Converter() = default;
  ~Converter() = default;

  template <typename T> auto operator()(T object);

private:
  Converter(Converter const &) = delete;
  Converter(Converter &&) = delete;
  Converter &operator=(Converter const &) = delete;
};
} // namespace NativePB