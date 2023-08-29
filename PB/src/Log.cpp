#include <Config.h>

namespace PB
{
void Log::printDebug(const char *format, va_list arglist) const
{
  if constexpr (Context::logLevel() == LogLevel::None ||
                Context::logLevel() > LogLevel::Debug) {
    return;
  }

  doPrintDebug(format, arglist);
}

void printDebug(const char *format, ...)
{
  va_list arglist;
  va_start(arglist, format);
  //sPrinter.printDebug(format, arglist);
  va_end(arglist);
}
void printInfo(const char *format, ...)
{
  va_list arglist;
  va_start(arglist, format);
  //sPrinter.printInfo(format, arglist);
  va_end(arglist);
}
void printWarning(const char *format, ...)
{
  va_list arglist;
  va_start(arglist, format);
  //sPrinter.printWarning(format, arglist);
  va_end(arglist);
}
void printError(const char *format, ...)
{
  va_list arglist;
  va_start(arglist, format);
  //sPrinter.printError(format, arglist);
  va_end(arglist);
}
}