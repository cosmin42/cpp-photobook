#pragma once

#include <Config.h>

namespace PB {

class Log {
public:
  virtual void printDebug(const char *format, ...) const = 0;
  virtual void printInfo(const char *format, ...) const = 0;
  virtual void printWarning(const char *format, ...) const = 0;
  virtual void printError(const char *format, ...) const = 0;

};

} // namespace PB