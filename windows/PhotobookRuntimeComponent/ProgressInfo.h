#pragma once

#include "ProgressInfo.g.h"

#include <pb/ProgressManager.h>

namespace winrt::PhotobookRuntimeComponent::implementation {
struct ProgressInfo : ProgressInfoT<ProgressInfo> {
  ProgressInfo() {}
  explicit ProgressInfo(PB::ProgressStatus progressStatus)
      : mProgressStatus(progressStatus)
  {
  }

  winrt::hstring Status()
  {
    return winrt::to_hstring(mProgressStatus.toString());
  }

private:
  PB::ProgressStatus mProgressStatus;
};
} // namespace winrt::PhotobookRuntimeComponent::implementation
