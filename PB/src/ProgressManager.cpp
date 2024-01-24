#include <pb/ProgressManager.h>

namespace PB {

void ProgressManager::configure(ProgressManagerListener *listener)
{
  mListener = listener;
}

void ProgressManager::subscribe(std::string name, JobType jobType,
                                int progressCap = 0)
{
  mProgress[name] = ProgressInfo{jobType, 0, progressCap};
  mListener->progressStarted(name, mProgress.at(name));
}

void ProgressManager::update(std::string name)
{
  auto progressInfo = mProgress.at(name);
  progressInfo.progress++;
  mProgress[name] = progressInfo;

  if (progressInfo.progressCap > 0 &&
      progressInfo.progressCap == progressInfo.progress) {
    finish(name);
  }

  mListener->progressUpdate(name, progressInfo);
}

void ProgressManager::abort(std::string name)
{
  mProgress.erase(name);
  mListener->progressAborted(name);
}

void ProgressManager::finish(std::string name) { mProgress.erase(name); }

ProgressInfo ProgressManager::totalDefiniteProgress() const
{
  ProgressInfo totalProgressInfo;

  totalProgressInfo.jobType = JobType::Full;

  for (auto [name, progressInfo] : mProgress) {
    if (progressInfo.progressCap > 0) {
      totalProgressInfo.progressCap += progressInfo.progressCap;
      totalProgressInfo.progress += progressInfo.progress;
    }
  }
  return totalProgressInfo;
}

ProgressInfo ProgressManager::totalIndefiniteProgress() const
{
  ProgressInfo totalProgressInfo;

  for (auto [name, progressInfo] : mProgress) {
    if (progressInfo.progressCap == 0) {
      totalProgressInfo.jobType = JobType::Full;
      totalProgressInfo.progress += progressInfo.progress;
    }
  }
  return totalProgressInfo;
}
} // namespace PB