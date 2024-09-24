#include <pb/components/TaskCruncher.h>

#include <pb/components/RuntimeUUID.h>

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

  auto taskCount = mapper.taskCount();
  auto progressId = mProgressService->start(progressName, taskCount);

  auto token = mStopSource.get_token();
  auto task = mapper.getTask(token);
  while (task.has_value()) {
    mPTC.at(poolName)->enqueue(
        [this, task{task}, &mapper, progressId{progressId}, token{token}]() {
          task->second();
          mProgressService->update(progressId);
          mapper.onTaskFinished(task->first);
        });

    task = mapper.getTask(mStopSource.get_token());
  }
}

void TaskCruncher::crunch(std::function<void()> f)
{
  mPTC.at("default")->enqueue(f);
}

void TaskCruncher::abort()
{
  mStopSource.request_stop();
  mProgressService->abortAll();
}

} // namespace PB
