#pragma once

#include <pb/DirectoryInspectionJob.h>
#include <pb/TaskCruncher.h>
#include <pb/util/Traits.h>

DECLARE_STRONG_STRING(ValidatorFunctionName)

namespace PB {

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
    mTaskCruncher->crunch("search-files", mJobs.at(id));
  }

private:
  DirectoryInspectionJobListener *mListener = nullptr;
  std::shared_ptr<TaskCruncher>   mTaskCruncher = nullptr;
  std::unordered_map<PBDev::DirectoryInspectionJobId, DirectoryInspectionJob,
                     boost::hash<PBDev::DirectoryInspectionJobId>>
      mJobs;
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
} // namespace PB
