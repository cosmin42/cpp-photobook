#pragma once

#include <common/Log.h>

#include <magic_enum.hpp>

namespace PB {

class StandardPrinter final : public Log {
public:
  StandardPrinter() = default;
  ~StandardPrinter() = default;

  void printDebug(const char *format, ...) const override
  {
    if constexpr (sLogLevel == LogLevel::None || sLogLevel > LogLevel::Debug) {
      return;
    }
    va_list arglist;
    va_start(arglist, format);

    constexpr auto tag = magic_enum::enum_name(LogLevel::Debug);
    printf("[%s] ", tag.data());
    printf(format, arglist);

    va_end(arglist);
  }
  void printInfo(const char *format, ...) const override
  {
    if constexpr (sLogLevel == LogLevel::None || sLogLevel > LogLevel::Info) {
      return;
    }
    va_list arglist;
    va_start(arglist, format);

    constexpr auto tag = magic_enum::enum_name(LogLevel::Info);
    printf("[%s] ", tag.data());
    printf(format, arglist);

    va_end(arglist);
  }
  void printWarning(const char *format, ...) const override
  {
    if constexpr (sLogLevel == LogLevel::None ||
                  sLogLevel > LogLevel::Warning) {
      return;
    }
    va_list arglist;
    va_start(arglist, format);

    constexpr auto tag = magic_enum::enum_name(LogLevel::Warning);
    printf("[%s] ", tag.data());
    printf(format, arglist);

    va_end(arglist);
  }
  void printError(const char *format, ...) const override
  {
    if constexpr (sLogLevel == LogLevel::None) {
      return;
    }
    va_list arglist;
    va_start(arglist, format);

    constexpr auto tag = magic_enum::enum_name(LogLevel::Error);
    printf("[%s] ", tag.data());
    printf(format, arglist);

    va_end(arglist);
  }

private:
  static constexpr LogLevel mLevel = sLogLevel;
};

} // namespace PB
