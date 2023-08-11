#pragma once

#include <string_view>

#include <util/Traits.h>

namespace PB {
enum class ErrorKind { Unknown, FileNotFound };

template <> struct PBValue<ErrorKind> {
  static constexpr std::string_view name = "ErrorKind";

  template <ErrorKind E> struct Members;

  template <> struct Members<ErrorKind::Unknown> {
    static constexpr std::string_view name = "Unknown";
  };

  template <> struct Members<ErrorKind::FileNotFound> {
    static constexpr std::string_view name = "FileNotFound";
  };
};
} // namespace PB