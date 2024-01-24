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
  mListener->progressUpdate(names(), totalDefiniteProgress(),
                            totalIndefiniteProgress());
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

  mListener->progressUpdate(names(), totalDefiniteProgress(),
                            totalIndefiniteProgress());
}

void ProgressManager::abort(std::string name)
{
  mProgress.erase(name);
  mListener->progressUpdate(names(), totalDefiniteProgress(),
                            totalIndefiniteProgress());
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

std::vector<std::string> ProgressManager::names() const
{
  std::vector<std::string> result;
  for (auto [name, progress] : mProgress) {
    result.push_back(name);
  }

  return result;
}
} // namespace PB