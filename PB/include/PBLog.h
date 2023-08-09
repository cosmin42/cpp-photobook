#pragma once

#include <LogLevel.h>

namespace PB {

class Printer {
public:
  virtual void printDebug(const char *format, ...) = 0;
  virtual void printInfo(const char *format, ...) = 0;
  virtual void printWarning(const char *format, ...) = 0;
  virtual void printError(const char *format, ...) = 0;

protected:
  constexpr LogLevel mLevel = LogLevel::None;
};

} // namespace PB