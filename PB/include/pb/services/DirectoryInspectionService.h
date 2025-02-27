#pragma once

#include <pb/infra/TaskCruncher.h>
#include <pb/jobs/DirectoryInspectionJob.h>
#include <pb/infra/Traits.h>

DECLARE_STRONG_STRING(ValidatorFunctionName)

using namespace PB::Job;

namespace PB::Service {

class DirectoryInspectionService final {
public:
  DirectoryInspectionService()
  {
    mValidators.emplace("lut-validator", lutValidator);
  }

  ~DirectoryInspectionService() = default;

  void configureTaskCruncher(std::shared_ptr<TaskCruncher> taskCruncher)
  {
    mTaskCruncher = taskCruncher;
  }

  void configureThreadScheduler(PBDev::ThreadScheduler *threadScheduler)
  {
    mThreadScheduler = threadScheduler;
  }

  void configureListener(DirectoryInspectionJobListener *listener)
  {
    mListener = listener;
  }

  void inspectDirectory(PBDev::DirectoryInspectionJobId id,
                        PBDev::ValidatorFunctionName validatorName, Path root)
  {
    mJobs.emplace(id, DirectoryInspectionJob(id, root));
    mJobs.at(id).configureListener(mListener);
    mJobs.at(id).setValidator(mValidators.at(validatorName));
    mJobs.at(id).configureThreadScheduler(mThreadScheduler);
    mStopSources[id] = mTaskCruncher->crunch(
        "search-files", mJobs.at(id), PBDev::ProgressJobName{"inspect-dir"});
  }

  bool isRunning(PBDev::DirectoryInspectionJobId id) const
  {
    return !mJobs.at(id).isFinished();
  }

  void stop(PBDev::DirectoryInspectionJobId id)
  {
    mStopSources.at(id).request_stop();
  }

private:
  DirectoryInspectionJobListener *mListener = nullptr;
  PBDev::ThreadScheduler         *mThreadScheduler = nullptr;
  std::shared_ptr<TaskCruncher>   mTaskCruncher = nullptr;
  std::unordered_map<PBDev::DirectoryInspectionJobId, DirectoryInspectionJob,
                     boost::hash<PBDev::DirectoryInspectionJobId>>
      mJobs;
  std::unordered_map<PBDev::DirectoryInspectionJobId, std::stop_source,
                     boost::hash<PBDev::DirectoryInspectionJobId>>
      mStopSources;
  std::unordered_map<PBDev::ValidatorFunctionName, std::function<bool(Path)>,
                     boost::hash<PBDev::ValidatorFunctionName>>
      mValidators;

  static bool lutValidator(Path path)
  {
    if (!std::filesystem::is_regular_file(path)) {
      return false;
    }
    return path.extension() == ".cube";
  }
};
} // namespace PB::Service
