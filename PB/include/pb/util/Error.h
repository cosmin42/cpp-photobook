#pragma once

#include <string>

#include <pb/Enums.h>

namespace PBDev {

class [[nodiscard]] Error final {
public:
  [[nodiscard]] auto kind() const noexcept -> PB::ErrorCode
  {
    return mErrorCode;
  }

  [[nodiscard]] auto description() const noexcept -> std::string
  {
    if (mDescription.empty()) {
      return defaultErrorDescription();
    }
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
  std::string              mDescription = "";
  PB::ErrorCode            mErrorCode = PB::ErrorCode::Unknown;
  [[maybe_unused]] uint8_t padding[4] = {0, 0, 0, 0};

  std::string defaultErrorDescription() const noexcept
  {
    switch (mErrorCode) {
    case PB::ErrorCode::Unknown:
      return "Unknown error.";
    case PB::ErrorCode::FileDoesNotExist: {
      return "The file doesn't exist.";
    }
    case PB::ErrorCode::NotADirectory: {
      return "Not a directory.";
    }
    case PB::ErrorCode::CorruptPersistenceFile: {
      return "Corrupt persistence file.";
    }
    case PB::ErrorCode::SQLiteError: {
      return "SQLite error.";
    }
    case PB::ErrorCode::FolderAlreadyImported: {
      return "Folder already imported.";
    }
    case PB::ErrorCode::JSONParseError: {
      return "JSON parse error.";
    }
    case PB::ErrorCode::NoImages: {
      return "No images.";
    }
    case PB::ErrorCode::CannotExport: {
      return "Cannot export.";
    }
    default:
      return "Unknown error";
    }
  }
};

} // namespace PBDev