#pragma once

#include <functional>
#include <unordered_set>

#include <pb/components/MapReducer.h>
#include <pb/infra/ThreadScheduler.h>
#include <pb/infra/Traits.h>

DECLARE_STRONG_UUID(DirectoryInspectionJobId)

namespace PB::Job {

class DirectoryInspectionJobListener {
public:
  virtual ~DirectoryInspectionJobListener() = default;
  virtual void onInspectionFinished(PBDev::DirectoryInspectionJobId id,
                                    std::vector<Path> searchResults) = 0;
};

class DirectoryInspectionJob final : public MapReducer {
public:
  DirectoryInspectionJob(PBDev::DirectoryInspectionJobId id, Path root)
      : mJobId(id), mRoot(root)
  {
    mRecursiveIterator = std::filesystem::recursive_directory_iterator(
        mRoot, std::filesystem::directory_options::skip_permission_denied);
  }

  ~DirectoryInspectionJob() = default;

  void configureListener(DirectoryInspectionJobListener *listener);

  void configureThreadScheduler(PBDev::ThreadScheduler *threadScheduler);

  // TODO: Improve this validator, it doesn't seem right
  void setValidator(std::function<bool(Path)> validator);

  std::optional<IdentifyableFunction>
  getTask(std::stop_token stopToken) override;

  void onTaskFinished(PBDev::MapReducerTaskId taskId) override;

  unsigned taskCount() const override { return 1; }

private:
  DirectoryInspectionJobListener               *mListener = nullptr;
  PBDev::ThreadScheduler                       *mThreadScheduler = nullptr;
  PBDev::DirectoryInspectionJobId               mJobId;
  Path                                          mRoot;
  std::filesystem::recursive_directory_iterator mRecursiveIterator;
  std::function<bool(Path)>                     mValidator = nullptr;
  std::vector<Path>                             mPaths;
  bool                                          mCrunchedFlag = false;
};
} // namespace PB::Job
