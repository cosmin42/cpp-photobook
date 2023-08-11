#pragma once

#include <common/LogLevel.h>

namespace PB {

class Log {
public:
  virtual void printDebug(const char *format, ...) = 0;
  virtual void printInfo(const char *format, ...) = 0;
  virtual void printWarning(const char *format, ...) = 0;
  virtual void printError(const char *format, ...) = 0;

};

} // namespace PB