#include <pb/ProgressManager.h>

namespace PB {

void ProgressManager::configure(ProgressManagerListener *listener)
{
  mListener = listener;
}

void ProgressManager::subscribe(std::string name, JobType jobType,
                                int progressCap)
{
  if (progressCap == 0) {
    mProgress[name] =
        ProgressInfo{jobType, ProgressType::Undefined, 0, progressCap};
  }
  else {
    mProgress[name] =
        ProgressInfo{jobType, ProgressType::Defined, 0, progressCap};
  }

  mListener->progressUpdate(totalDefiniteProgress(), totalIndefiniteProgress());
}

void ProgressManager::update(std::string name)
{
  auto &progressInfo = mProgress.at(name);
  progressInfo.progress++;

  if (progressInfo.progressCap > 0 &&
      progressInfo.progressCap == progressInfo.progress) {
    finish(name);
  }

  mListener->progressUpdate(totalDefiniteProgress(), totalIndefiniteProgress());
}

void ProgressManager::abort(std::string name)
{
  mProgress.erase(name);

  mListener->progressUpdate(totalDefiniteProgress(), totalIndefiniteProgress());
}

void ProgressManager::finish(std::string name)
{
  mProgress.erase(name);
  mListener->progressUpdate(totalDefiniteProgress(), totalIndefiniteProgress());
}

ProgressInfo ProgressManager::totalDefiniteProgress() const
{
  ProgressInfo totalProgressInfo;

  totalProgressInfo.jobType = JobType::Full;

  for (auto [name, progressInfo] : mProgress) {
    PBDev::basicAssert(progressInfo.progressType != ProgressType::None);
    if (progressInfo.progressType == ProgressType::Defined) {
      totalProgressInfo.progressCap += progressInfo.progressCap;
      totalProgressInfo.progress += progressInfo.progress;
      totalProgressInfo.jobsProgress.push_back(name);
    }
  }
  return totalProgressInfo;
}

ProgressInfo ProgressManager::totalIndefiniteProgress() const
{
  ProgressInfo totalProgressInfo;
  totalProgressInfo.jobType = JobType::Full;
  for (auto [name, progressInfo] : mProgress) {
    if (progressInfo.progressCap == 0) {
      totalProgressInfo.progress += progressInfo.progress;
      if (progressInfo.jobType == JobType::Map) {
        totalProgressInfo.jobsProgress.push_back(
            Path(name).filename().string());
      }
      else if (progressInfo.jobType == JobType::ThumbnailsProcess) {
        totalProgressInfo.jobsProgress.push_back(
            Path(name).filename().string());
      }
      else {
        totalProgressInfo.jobsProgress.push_back(name);
      }
    }
  }
  return totalProgressInfo;
}

} // namespace PB