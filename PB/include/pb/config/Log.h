#pragma once

#include <pb/Enums.h>
#include <cstdarg>

namespace PB {

class Context;

class Log {
public:
  virtual ~Log() = default;

  void printDebug(const char *format, va_list arglist) const;

  void printInfo(const char *format, va_list arglist) const;

  void printWarning(const char *format, va_list arglist) const;

  void printError(const char *format, va_list arglist) const;

private:
  virtual void doPrintDebug(const char *format, va_list arglist) const = 0;
  virtual void doPrintInfo(const char *format, va_list arglist) const = 0;
  virtual void doPrintWarning(const char *format, va_list arglist) const = 0;
  virtual void doPrintError(const char *format, va_list arglist) const = 0;
};

void printDebug(const char *format, ...);
void printInfo(const char *format, ...);
void printWarning(const char *format, ...);
void printError(const char *format, ...);

struct TimerPrinter {
  static void onClose(double duration);
};

} // namespace PB