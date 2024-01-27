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

  auto tasksNames = names();

  mListener->progressUpdate(tasksNames.first, tasksNames.second,
                            totalDefiniteProgress(), totalIndefiniteProgress());
}

void ProgressManager::update(std::string name)
{
  auto &progressInfo = mProgress.at(name);
  progressInfo.progress++;

  if (progressInfo.progressCap > 0 &&
      progressInfo.progressCap == progressInfo.progress) {
    finish(name);
  }

  auto tasksNames = names();

  mListener->progressUpdate(tasksNames.first, tasksNames.second,
                            totalDefiniteProgress(), totalIndefiniteProgress());
}

void ProgressManager::abort(std::string name)
{
  mProgress.erase(name);

  auto tasksNames = names();

  mListener->progressUpdate(tasksNames.first, tasksNames.second,
                            totalDefiniteProgress(), totalIndefiniteProgress());
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

std::pair<std::vector<std::string>, std::vector<std::string>>
ProgressManager::names() const
{
  std::vector<std::string> definedNames;
  std::vector<std::string> undefinedNames;
  for (auto [name, progress] : mProgress) {
    if (progress.progressType == ProgressType::Defined) {
      if (progress.jobType == JobType::Map) {
        definedNames.push_back(Path(name).filename().string());
      }
      else {
        definedNames.push_back(name);
      }
    }
    else {
      undefinedNames.push_back(name);
    }
  }

  return {definedNames, undefinedNames};
}
} // namespace PB