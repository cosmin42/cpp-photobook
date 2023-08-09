#pragma once

#include <string_view>

#include <util/Traits.h>

namespace PB {

enum class LogLevel { None, Debug, Info, Warning, Error };

template <> struct PBValue<LogLevel> {
  static constexpr std::string_view name = "LogLevel";

  template <LogLevel L> struct Members;

  template <> struct Members<LogLevel::None> {
    static constexpr std::string_view name = "None";
  };

  template <> struct Members<LogLevel::Debug> {
    static constexpr std::string_view name = "Debug";
  };

  template <> struct Members<LogLevel::Info> {
    static constexpr std::string_view name = "Info";
  };

  template <> struct Members<LogLevel::Warning> {
    static constexpr std::string_view name = "Warning";
  };

  template <> struct Members<LogLevel::Error> {
    static constexpr std::string_view name = "Error";
  };
};

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