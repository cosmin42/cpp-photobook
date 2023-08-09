#pragma once

#include <ErrorTypes.h>

namespace PB {

class Error final {
public:
  Error() = default;
  ~Error() = default;

  auto kind() const -> ErrorKind { return mErrorKind; }

  auto description() const -> std::string { return mDescription; }

  auto operator<<(ErrorKind &kind) -> Error { mErrorKind = kind; }
  auto operator<<(std::string description) -> Error {
    mDescription = description;
  }

private:
  ErrorKind mErrorKind = ErrorKind::Unknown;
  std::string mDescription;
};

} // namespace PB