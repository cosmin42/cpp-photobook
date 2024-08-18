#pragma once

#include <functional>
#include <unordered_set>

#include <pb/MapReducer.h>
#include <pb/util/Traits.h>

DECLARE_STRONG_UUID(DirectoryInspectionJobId)

namespace PB {

class DirectoryInspectionJobListener {
public:
  virtual ~DirectoryInspectionJobListener() = default;
  virtual void onFoundFile(Path file) = 0;
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

  void setDirectoryInspectionServiceListener(
      DirectoryInspectionJobListener *listener)
  {
    mListener = listener;
  }

  void setValidator(std::function<bool(Path)> validator)
  {
    mValidator = validator;
  }

  std::optional<IdentifyableFunction>
  getNext(std::stop_token stopToken) override
  {
    if (mRecursiveIterator == std::filesystem::end(mRecursiveIterator)) {
      return std::nullopt;
    }
    auto id = PBDev::MapReducerTaskId(RuntimeUUID::newUUID());
    mInProgressTaskIds.insert(id);
    return IdentifyableFunction{id, [this]() {
                                  auto path = mRecursiveIterator->path();
                                  if (mValidator(path)) {
                                    mListener->onFoundFile(path);
                                    mPaths.push_back(path);
                                  }
                                  mRecursiveIterator++;
                                }};
  }

  void onFinished(PBDev::MapReducerTaskId taskId) override
  {
    PBDev::basicAssert(mInProgressTaskIds.contains(taskId));
    mInProgressTaskIds.erase(taskId);

    if (mRecursiveIterator == std::filesystem::end(mRecursiveIterator) &&
        mInProgressTaskIds.empty()) {
      mListener->onInspectionFinished(mJobId, mPaths);
    }
  }

private:
  DirectoryInspectionJobListener               *mListener = nullptr;
  PBDev::DirectoryInspectionJobId               mJobId;
  Path                                          mRoot;
  std::filesystem::recursive_directory_iterator mRecursiveIterator;
  std::function<bool(Path)>                     mValidator = nullptr;
  std::vector<Path>                             mPaths;
  std::unordered_set<PBDev::MapReducerTaskId,
                     boost::hash<PBDev::MapReducerTaskId>>
      mInProgressTaskIds;
};
} // namespace PB
