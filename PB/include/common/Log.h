#pragma once

#include <cstdarg>
#include <common/LogLevel.h>

namespace PB {

class Context;

class Log {
public:
  void printDebug(const char *format, va_list arglist) const;

  void printInfo(const char *format, va_list arglist) const
  {
    doPrintInfo(format, arglist);
  }
  void printWarning(const char *format, va_list arglist) const
  {
    doPrintWarning(format, arglist);
  }
  void printError(const char *format, va_list arglist) const
  {
    doPrintError(format, arglist);
  }

private:
  virtual void doPrintDebug(const char *format, va_list arglist) const = 0;
  virtual void doPrintInfo(const char *format, va_list arglist) const = 0;
  virtual void doPrintWarning(const char *format, va_list arglist) const = 0;
  virtual void doPrintError(const char *format, va_list arglist) const = 0;
};

} // namespace PB