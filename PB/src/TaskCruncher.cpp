#include <pb/components/TaskCruncher.h>

#include <pb/components/RuntimeUUID.h>

namespace PB {
void TaskCruncher::registerPTC(const std::string poolName,
                               unsigned          threadsCount)
{
  mPTC.emplace(poolName,
               std::make_unique<PBDev::ParallelTaskConsumer>(threadsCount));

  mStopSources[poolName] = std::stop_source();
}

void TaskCruncher::crunch(const std::string poolName, MapReducer &mapper,
                          PBDev::ProgressJobName progressName)
{
  PBDev::basicAssert(mPTC.find(poolName) != mPTC.end());

  auto taskCount = mapper.taskCount();
  auto progressId = mProgressService->start(progressName, taskCount);

  mProgressNames.emplace(poolName, progressId);

  auto token = mStopSources.at(poolName).get_token();
  auto task = mapper.getTask(token);
  while (task.has_value()) {
    mPTC.at(poolName)->enqueue(
        [this, task{task}, &mapper, progressId{progressId}, token{token}]() {
          task->second();
          mProgressService->update(progressId);
          mapper.onTaskFinishedInternal(task->first);
        });

    task = mapper.getTask(mStopSources.at(poolName).get_token());
  }
}

void TaskCruncher::crunch(std::function<void()> f)
{
  mPTC.at("default")->enqueue(f);
}

void TaskCruncher::abort()
{
  for (auto &[name, stopSource] : mStopSources) {
    stopSource.request_stop();
    mPTC.at(name)->wait();
  }
  mProgressService->abortAll();
}
} // namespace PB
