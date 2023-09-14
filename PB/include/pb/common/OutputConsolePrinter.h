#pragma once

#include <windows.h>

#include <magic_enum.hpp>

#include <pb/common/Log.h>

namespace PB {

class OutputConsolePrinter final : public Log {

  static constexpr unsigned MAX_PRINT_BUFFER_SIZE = 512;

public:
  OutputConsolePrinter() = default;
  ~OutputConsolePrinter() = default;

  void doPrintDebug(const char *format, va_list arglist) const override
  {
    constexpr auto tag = magic_enum::enum_name(LogLevel::Debug);
    char           buffer[MAX_PRINT_BUFFER_SIZE];

    auto      len = _vscprintf(format, arglist);
    auto ret = vsprintf_s(buffer, len, format, arglist);

    assert(ret >= 0);

    OutputDebugStringA(("[" + std::string(tag) + "] " + buffer).c_str());
  }
  void doPrintInfo(const char *format, va_list arglist) const override
  {
    constexpr auto tag = magic_enum::enum_name(LogLevel::Info);
    char           buffer[MAX_PRINT_BUFFER_SIZE];
    auto           len = _vscprintf(format, arglist);
    auto           ret = vsprintf_s(buffer, len, format, arglist);

    assert(ret >= 0);

    OutputDebugStringA(("[" + std::string(tag) + "] " + buffer).c_str());
  }
  void doPrintWarning(const char *format, va_list arglist) const override
  {
    constexpr auto tag = magic_enum::enum_name(LogLevel::Warning);
    char           buffer[MAX_PRINT_BUFFER_SIZE];
    auto           len = _vscprintf(format, arglist);
    auto           ret = vsprintf_s(buffer, len, format, arglist);

    assert(ret >= 0);

    OutputDebugStringA(("[" + std::string(tag) + "] " + buffer).c_str());
  }
  void doPrintError(const char *format, va_list arglist) const override
  {
    constexpr auto tag = magic_enum::enum_name(LogLevel::Error);
    char           buffer[MAX_PRINT_BUFFER_SIZE];
    auto           len = _vscprintf(format, arglist);
    auto           ret = vsprintf_s(buffer, len, format, arglist);

    assert(ret >= 0);

    OutputDebugStringA(("[" + std::string(tag) + "] " + buffer).c_str());
  }
};

} // namespace PB
