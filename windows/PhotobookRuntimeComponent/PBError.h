#pragma once

#include "PBError.g.h"

#include <pb/infra/Error.h>

namespace winrt::PhotobookRuntimeComponent::implementation {
struct PBError : PBErrorT<PBError> {
  PBError() {}
  explicit PBError(PBDev::Error error) : mError(error) {}

  winrt::hstring description()
  {
    return winrt::to_hstring(mError.description());
  }
  PBErrorCode errorCode() { return (PBErrorCode)mError.kind(); }

private:
  PBDev::Error mError;
};
} // namespace winrt::PhotobookRuntimeComponent::implementation
