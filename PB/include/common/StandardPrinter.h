#pragma once




#include <magic_enum.hpp>

#include <common/Log.h>

namespace PB {

class StandardPrinter final : public Log {
public:
  StandardPrinter() = default;
  ~StandardPrinter() = default;

  void printDebug(const char *format, va_list arglist) const override
  {
    if constexpr (sLogLevel == LogLevel::None || sLogLevel > LogLevel::Debug) {
      return;
    }

    constexpr auto tag = magic_enum::enum_name(LogLevel::Debug);
    printf("[%s] ", tag.data());
    vprintf(format, arglist);
  }
  void printInfo(const char *format, va_list arglist) const override
  {
    if constexpr (sLogLevel == LogLevel::None || sLogLevel > LogLevel::Info) {
      return;
    }

    constexpr auto tag = magic_enum::enum_name(LogLevel::Info);
    printf("[%s] ", tag.data());
    vprintf(format, arglist);
  }
  void printWarning(const char *format, va_list arglist) const override
  {
    if constexpr (sLogLevel == LogLevel::None ||
                  sLogLevel > LogLevel::Warning) {
      return;
    }

    constexpr auto tag = magic_enum::enum_name(LogLevel::Warning);
    printf("[%s] ", tag.data());
    vprintf(format, arglist);
  }
  void printError(const char *format, va_list arglist) const override
  {
    if constexpr (sLogLevel == LogLevel::None) {
      return;
    }

    constexpr auto tag = magic_enum::enum_name(LogLevel::Error);
    printf("[%s] ", tag.data());
    vprintf(format, arglist);
  }

private:
  static constexpr LogLevel mLevel = sLogLevel;
};

} // namespace PB
