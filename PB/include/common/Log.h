#pragma once

#include <cstdarg>

#include <Config.h>

namespace PB {

class Log {
public:
  virtual void printDebug(const char *format, va_list arglist) const = 0;
  virtual void printInfo(const char *format, va_list arglist) const = 0;
  virtual void printWarning(const char *format, va_list arglist) const = 0;
  virtual void printError(const char *format, va_list arglist) const = 0;

};

} // namespace PB