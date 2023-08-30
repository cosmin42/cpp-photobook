#pragma once

#include <pb/ErrorTypes.h>

namespace PB {

class [[nodiscard]] Error final {
public:
  Error() = default;
  ~Error() = default;

  [[nodiscard]] auto kind() const noexcept -> ErrorKind { return mErrorKind; }

  [[nodiscard]] auto description() const noexcept -> std::string {
    return mDescription;
  }

  auto operator<<(ErrorKind kind) noexcept -> Error {
    mErrorKind = kind;
    return *this;
  }
  auto operator<<(std::string description) noexcept -> Error {
    mDescription = description;
    return *this;
  }

private:
  ErrorKind mErrorKind = ErrorKind::Unknown;
  std::string mDescription;
};

} // namespace PB