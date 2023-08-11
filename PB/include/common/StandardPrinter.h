#pragma once

#include <common/Log.h>

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
    printf(
        "[%s] ",
        PB::PBValue<PB::LogLevel>::Members<PB::LogLevel::Debug>::name.data());
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
    printf("[%s] ",
           PB::PBValue<PB::LogLevel>::Members<PB::LogLevel::Info>::name.data());
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
    printf(
        "[%s] ",
        PB::PBValue<PB::LogLevel>::Members<PB::LogLevel::Warning>::name.data());
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
    printf(
        "[%s] ",
        PB::PBValue<PB::LogLevel>::Members<PB::LogLevel::Error>::name.data());
    printf(format, arglist);
    va_end(arglist);
  }

private:
  static constexpr LogLevel mLevel = sLogLevel;
};

} // namespace PB
