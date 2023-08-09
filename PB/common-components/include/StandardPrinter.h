#pragma once

#include <PBLog.h>

namespace CC {

class StandardPrinter final : public PB::Printer {
public:
  SpecificPrinter() = default;
  ~SpecificPrinter() = default;

  void printDebug(const char *format, ...) override {}
  void printInfo(const char *format, ...) override {}
  void printWarning(const char *format, ...) override {}
  void printError(const char *format, ...) override {}
};

} // namespace CC
