#include <Config.h>

namespace PB {
void Log::printDebug(const char *format, va_list arglist) const
{
  if constexpr (Context::logLevel() == LogLevel::None ||
                Context::logLevel() > LogLevel::Debug) {
    return;
  }

  doPrintDebug(format, arglist);
}

void Log::printInfo(const char *format, va_list arglist) const
{
  if constexpr (Context::logLevel() == LogLevel::None ||
                Context::logLevel() > LogLevel::Info) {
    return;
  }

  doPrintInfo(format, arglist);
}

void Log::printWarning(const char *format, va_list arglist) const
{
  if constexpr (Context::logLevel() == LogLevel::None ||
                Context::logLevel() > LogLevel::Warning) {
    return;
  }

  doPrintWarning(format, arglist);
}

void Log::printError(const char *format, va_list arglist) const
{
  if constexpr (Context::logLevel() == LogLevel::None) {
    return;
  }

  doPrintError(format, arglist);
}

void printDebug(const char *format, ...)
{
  va_list arglist;
  va_start(arglist, format);
  Context::inst().printer().printDebug(format, arglist);
  va_end(arglist);
}
void printInfo(const char *format, ...)
{
  va_list arglist;
  va_start(arglist, format);
  Context::inst().printer().printInfo(format, arglist);
  va_end(arglist);
}
void printWarning(const char *format, ...)
{
  va_list arglist;
  va_start(arglist, format);
  Context::inst().printer().printWarning(format, arglist);
  va_end(arglist);
}
void printError(const char *format, ...)
{
  va_list arglist;
  va_start(arglist, format);
  Context::inst().printer().printError(format, arglist);
  va_end(arglist);
}

void TimerPrinter::operator()(double duration)
{
  printDebug("Duration: %ld", duration);
}

} // namespace PB