#pragma once

#include <string>

#include <pb/Enums.h>

namespace PB {

class [[nodiscard]] Error final {
public:
  [[nodiscard]] auto kind() const noexcept -> ErrorCode { return mErrorCode; }

  [[nodiscard]] auto description() const noexcept -> std::string
  {
    return mDescription;
  }

  auto operator<<(ErrorCode kind) noexcept -> Error
  {
    mErrorCode = kind;
    return *this;
  }
  auto operator<<(std::string description) noexcept -> Error
  {
    mDescription = description;
    return *this;
  }

private:
  std::string mDescription = "";
  ErrorCode   mErrorCode = ErrorCode::Unknown;
  [[maybe_unused]] uint8_t     padding[4];
};

} // namespace PB