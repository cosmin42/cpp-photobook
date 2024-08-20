#include <pb/TaskCruncher.h>

#include <pb/RuntimeUUID.h>

namespace PB {
void TaskCruncher::registerPTC(const std::string poolName,
                               unsigned          threadsCount)
{
  mPTC.emplace(poolName,
               std::make_unique<PBDev::ParallelTaskConsumer>(threadsCount));
}

void TaskCruncher::crunch(const std::string poolName, MapReducer &mapper,
                          PBDev::ProgressJobName progressName)
{
  PBDev::basicAssert(mPTC.find(poolName) != mPTC.end());

  auto progressId = mProgressManager->start(progressName, mapper.taskCount());

  auto task = mapper.getTask(mStopSource.get_token());
  while (task.has_value()) {

    mPTC.at(poolName)->enqueue(
        [this, task{task}, &mapper, progressId{progressId}]() {
          task->second();
          mapper.onTaskFinished(task->first);
          mProgressManager->update(progressId);
        });

    task = mapper.getTask(mStopSource.get_token());
  }
}

void TaskCruncher::abort() { mStopSource.request_stop(); }

} // namespace PB
