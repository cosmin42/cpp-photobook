#pragma once

#include <string>

#include <pb/ErrorTypes.h>

namespace PB {

class [[nodiscard]] Error final {
public:
  Error() = default;
  Error(Error const &other)
      : mDescription(other.mDescription), mErrorKind(mErrorKind)
  {
  }

  Error(Error &&other) noexcept
      : mDescription(other.mDescription), mErrorKind(mErrorKind)
  {
  }

  Error &operator=(Error const &other)
  {
    mDescription = other.mDescription;
    mErrorKind = other.mErrorKind;
    return *this;
  }
  ~Error() = default;

  [[nodiscard]] auto kind() const noexcept -> ErrorKind { return mErrorKind; }

  [[nodiscard]] auto description() const noexcept -> std::string
  {
    return mDescription;
  }

  auto operator<<(ErrorKind kind) noexcept -> Error
  {
    mErrorKind = kind;
    return *this;
  }
  auto operator<<(std::string description) noexcept -> Error
  {
    mDescription = description;
    return *this;
  }

private:
  std::string mDescription = "";
  ErrorKind   mErrorKind = ErrorKind::Unknown;
  uint8_t     padding[4];
};

} // namespace PB