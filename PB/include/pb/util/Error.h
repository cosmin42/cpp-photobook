#pragma once

#include <string>

#include <pb/Enums.h>

namespace PBDev {

class [[nodiscard]] Error final {
public:
  [[nodiscard]] auto kind() const noexcept -> PB::ErrorCode { return mErrorCode; }

  [[nodiscard]] auto description() const noexcept -> std::string
  {
    return mDescription;
  }

  auto operator<<(PB::ErrorCode kind) noexcept -> Error
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
  PB::ErrorCode   mErrorCode = PB::ErrorCode::Unknown;
  [[maybe_unused]] uint8_t padding[4] = {0, 0, 0, 0};
};

} // namespace PB