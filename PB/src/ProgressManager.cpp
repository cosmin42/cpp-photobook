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

  mListener->progressUpdate(name, progressInfo);
}

void ProgressManager::abort(std::string name)
{
  mProgress.erase(name);
  mListener->progressAborted(name);
}
} // namespace PB