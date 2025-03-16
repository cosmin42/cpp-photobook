#include <pb/services/DirectoryInspectionService.h>


namespace PB::Service {
DirectoryInspectionService::DirectoryInspectionService()
{
  mValidators.emplace("lut-validator", lutValidator);
}

void DirectoryInspectionService::configureTaskCruncher(
    std::shared_ptr<TaskCruncher> taskCruncher)
{
  mTaskCruncher = taskCruncher;
}

void DirectoryInspectionService::configureThreadScheduler(
    PBDev::ThreadScheduler *threadScheduler)
{
  mThreadScheduler = threadScheduler;
}

void DirectoryInspectionService::configureListener(
    DirectoryInspectionJobListener *listener)
{
  mListener = listener;
}

void DirectoryInspectionService::inspectDirectory(
    PBDev::DirectoryInspectionJobId id,
                      PBDev::ValidatorFunctionName validatorName, Path root)
{
  mJobs.emplace(id, DirectoryInspectionJob(id, root));
  mJobs.at(id).configureListener(mListener);
  mJobs.at(id).setValidator(mValidators.at(validatorName));
  mJobs.at(id).configureThreadScheduler(mThreadScheduler);
  mStopSources[id] = mTaskCruncher->crunch(
      "search-files", mJobs.at(id), PBDev::ProgressJobName{"inspect-dir"});
}

bool DirectoryInspectionService::isRunning(
    PBDev::DirectoryInspectionJobId id) const
{
  return !mJobs.at(id).isFinished();
}

void DirectoryInspectionService::stop(PBDev::DirectoryInspectionJobId id)
{
  mStopSources.at(id).request_stop();
}

bool DirectoryInspectionService::lutValidator(Path path)
{
  if (!std::filesystem::is_regular_file(path)) {
    return false;
  }
  return path.extension() == ".cube";
}

}
