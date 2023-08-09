#pragma once

#include <string_view>

#include <util/Traits.h>

namespace PB {

enum class LogLevel { None, Debug, Info, Warning, Error };

template <> struct PBValue<LogLevel::None> {
  static constexpr std::string_view str = "None";
}

} // namespace PB