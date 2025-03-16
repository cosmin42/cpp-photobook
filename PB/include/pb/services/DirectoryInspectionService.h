#pragma once

#include <pb/infra/TaskCruncher.h>
#include <pb/infra/Traits.h>
#include <pb/jobs/DirectoryInspectionJob.h>

DECLARE_STRONG_STRING(ValidatorFunctionName)

using namespace PB::Job;

namespace PB::Service {

class DirectoryInspectionService final {
public:
  DirectoryInspectionService();

  ~DirectoryInspectionService() = default;

  void configureTaskCruncher(std::shared_ptr<TaskCruncher> taskCruncher);

  void configureThreadScheduler(PBDev::ThreadScheduler *threadScheduler);

  void configureListener(DirectoryInspectionJobListener *listener);

  void inspectDirectory(PBDev::DirectoryInspectionJobId id,
                        PBDev::ValidatorFunctionName validatorName, Path root);

  bool isRunning(PBDev::DirectoryInspectionJobId id) const;
  void stop(PBDev::DirectoryInspectionJobId id);

private:
  static bool lutValidator(Path path);

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
};
} // namespace PB::Service
