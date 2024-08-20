#pragma once

#include "ProgressInfo.g.h"

#include <pb/ProgressManager.h>

namespace winrt::PhotobookRuntimeComponent::implementation {
struct ProgressInfo : ProgressInfoT<ProgressInfo> {
  ProgressInfo() {}
  explicit ProgressInfo(PB::ProgressInfo progressInfo)
      : mProgressInfo(progressInfo)
  {
  }

  int progress() { return mProgressInfo.progress; }

  int progressCap() { return mProgressInfo.progressCap; }

  Windows::Foundation::Collections::IVector<winrt::hstring> jobsProgress()
  {
    auto nativeJobsProgress = winrt::single_threaded_vector<winrt::hstring>();


    return nativeJobsProgress;
  }

private:
  PB::ProgressInfo mProgressInfo;
};
} // namespace winrt::PhotobookRuntimeComponent::implementation
