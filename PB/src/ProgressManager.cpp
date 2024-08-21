#include <pb/ProgressManager.h>

namespace PB {

void ProgressManager::configure(ProgressManagerListener *listener)
{
  mListener = listener;
}

void ProgressManager::configureScheduler(PBDev::ThreadScheduler *scheduler)
{
  mScheduler = scheduler;
}

PBDev::ProgressId ProgressManager::start(PBDev::ProgressJobName name,
                                         unsigned               taskCount)
{
  PBDev::ProgressId id(RuntimeUUID::newUUID());
  mScheduler->post([this, id{id}, name{name}, taskCount{taskCount}]() {
    mProgressData.emplace(id, ProgressInfo{name, taskCount, 0});
    auto aggregateStatus = this->aggregateStatus();
    mListener->progressUpdate(aggregateStatus);
  });
  return id;
}

void ProgressManager::update(PBDev::ProgressId id)
{
  mScheduler->post([this, id]() {
    mProgressData[id].progress++;
    auto aggregate = aggregateStatus();
    mListener->progressUpdate(aggregate);
    if (mProgressData.at(id).progress == mProgressData.at(id).progressCap) {
      finish(id);
    }
  });
}

void ProgressManager::finish(PBDev::ProgressId id)
{
  mScheduler->post([this, id]() {
    mProgressData.erase(id);
    mListener->progressUpdate(aggregateStatus());
  });
}

void ProgressManager::abortAll()
{
  mScheduler->post([this]() {
    mProgressData.clear();
    mListener->progressUpdate(aggregateStatus());
  });
}

ProgressStatus ProgressManager::aggregateStatus() const
{
  // TODO: Make this function beautiful
  ProgressStatus status;

  std::unordered_set<PBDev::ProgressJobName,
                     boost::hash<PBDev::ProgressJobName>>
      knownJobs;
  std::unordered_set<PBDev::ProgressJobName,
                     boost::hash<PBDev::ProgressJobName>>
      unknownJobs;
  for (auto [id, progressInfo] : mProgressData) {
    if (unknownJobs.contains(progressInfo.name)) {
      continue;
    }
    else {
      if (progressInfo.progressCap == 0) {
        unknownJobs.insert(progressInfo.name);
        if (knownJobs.contains(progressInfo.name)) {
          knownJobs.erase(progressInfo.name);
        }
      }
      else {
        knownJobs.insert(progressInfo.name);
      }
    }
  }
  std::unordered_map<PBDev::ProgressJobName, int,
                     boost::hash<PBDev::ProgressJobName>>
      maxCapSum;
  std::unordered_map<PBDev::ProgressJobName, int,
                     boost::hash<PBDev::ProgressJobName>>
      progressSum;

  for (auto const &[id, progressInfo] : mProgressData) {
    if (knownJobs.contains(progressInfo.name)) {
      maxCapSum[progressInfo.name] += progressInfo.progressCap;
      progressSum[progressInfo.name] += progressInfo.progress;
    }
    else if (unknownJobs.contains(progressInfo.name)) {
      status.statusMap[progressInfo.name] = std::nullopt;
    }
    else {
      PBDev::basicAssert(false);
    }
  }

  for (auto const &[jobName, progress] : progressSum) {
    status.statusMap[jobName] =
        (float)(progress * 100) / (float)maxCapSum.at(jobName);
  }
  return status;
}

} // namespace PB