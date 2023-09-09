#pragma once

#include <winrt/base.h>

namespace NativePB {
class Converter final {
public:
  Converter() = default;
  ~Converter() = default;

  std::string operator()(const winrt::hstring &object)
  {
    std::string result = winrt::to_string(object);
    return result;
  }

private:
  Converter(Converter const &) = delete;
  Converter(Converter &&) = delete;
  Converter &operator=(Converter const &) = delete;
};
} // namespace NativePB