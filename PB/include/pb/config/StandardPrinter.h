#pragma once

#include <magic_enum.hpp>

#include <pb/config/Log.h>

namespace PB {

class StandardPrinter final : public Log {
public:
  StandardPrinter() = default;
  ~StandardPrinter() = default;

  void doPrintDebug(const char *format, va_list arglist) const override
  {
    constexpr auto tag = magic_enum::enum_name(LogLevel::Debug);
    printf("[%s] ", tag.data());
    vprintf(format, arglist);
  }
  void doPrintInfo(const char *format, va_list arglist) const override
  {
    constexpr auto tag = magic_enum::enum_name(LogLevel::Info);
    printf("[%s] ", tag.data());
    vprintf(format, arglist);
  }
  void doPrintWarning(const char *format, va_list arglist) const override
  {
    constexpr auto tag = magic_enum::enum_name(LogLevel::Warning);
    printf("[%s] ", tag.data());
    vprintf(format, arglist);
  }
  void doPrintError(const char *format, va_list arglist) const override
  {
    constexpr auto tag = magic_enum::enum_name(LogLevel::Error);
    printf("[%s] ", tag.data());
    vprintf(format, arglist);
  }
};

} // namespace PB
