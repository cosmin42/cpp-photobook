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

  JobType jobType() { return (JobType)mProgressInfo.jobType; }

  ProgressType progressType()
  {
    return (ProgressType)mProgressInfo.progressType;
  }

  int progress() { return mProgressInfo.progress; }

  int progressCap() { return mProgressInfo.progressCap; }

  Windows::Foundation::Collections::IVector<winrt::hstring> jobsProgress()
  {
    auto nativeJobsProgress = winrt::single_threaded_vector<winrt::hstring>();

    for (auto i = 0; i < mProgressInfo.jobsProgress.size(); ++i) {
      nativeJobsProgress.Append(
          winrt::to_hstring(mProgressInfo.jobsProgress.at(i)));
    }

    return nativeJobsProgress;
  }

private:
  PB::ProgressInfo mProgressInfo;
};
} // namespace winrt::PhotobookRuntimeComponent::implementation
