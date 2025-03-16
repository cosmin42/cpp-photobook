#include <pb/jobs/DirectoryInspectionJob.h>

#pragma warning(push)
#pragma warning(disable : 4996)
#include <spdlog/spdlog.h>
#pragma warning(pop)

#include <boost/uuid/uuid_io.hpp>

#include <pb/Config.h>

namespace PB::Job {

void DirectoryInspectionJob::configureListener(
    DirectoryInspectionJobListener *listener)
{
  mListener = listener;
}

void DirectoryInspectionJob::configureThreadScheduler(
    PBDev::ThreadScheduler *threadScheduler)
{
  mThreadScheduler = threadScheduler;
}

void DirectoryInspectionJob::setValidator(std::function<bool(Path)> validator)
{
  mValidator = validator;
}

std::optional<IdentifyableFunction>
DirectoryInspectionJob::getTask(std::stop_token stopToken)
{
  if (mCrunchedFlag) {
    return std::nullopt;
  }

  IdentifyableFunction f{
      RuntimeUUID::newUUID(), [this, stopToken{stopToken}]() {
        auto recursiveIterator = std::filesystem::recursive_directory_iterator(
            mRoot, std::filesystem::directory_options::skip_permission_denied);
        while (recursiveIterator != std::filesystem::end(recursiveIterator) &&
               !stopToken.stop_requested()) {
          auto path = recursiveIterator->path();
          if (mValidator(path)) {
            mPaths.push_back(path);
          }
          recursiveIterator++;
        }
      }};
  mCrunchedFlag = true;
  return f;
}

void DirectoryInspectionJob::onTaskFinished(PBDev::MapReducerTaskId taskId)
{
  UNUSED(taskId);
  mListener->onInspectionFinished(mJobId, mPaths);
}

} // namespace PB::Job
