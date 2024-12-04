#include <pb/components/TaskCruncher.h>

#include <pb/components/RuntimeUUID.h>

namespace PB {
void TaskCruncher::registerPTC(const std::string poolName,
                               unsigned          threadsCount)
{
  mPTC.emplace(poolName,
               std::make_unique<PBDev::ParallelTaskConsumer>(threadsCount));
}

std::stop_source TaskCruncher::crunch(const std::string      poolName,
                                      MapReducer            &mapper,
                                      PBDev::ProgressJobName progressName)
{
  PBDev::basicAssert(mPTC.find(poolName) != mPTC.end());

  std::stop_source stopSource;

  auto taskCount = mapper.taskCount();
  auto progressId = mProgressService->start(progressName, taskCount);

  mProgressNames.emplace(poolName, progressId);

  auto task = mapper.getTask(stopSource.get_token());
  while (task.has_value()) {
    mPTC.at(poolName)->enqueue(
        [this, task{task}, &mapper, progressId{progressId}]() {
          task->second();
          mProgressService->update(progressId);
          mapper.onTaskFinishedInternal(task->first);
        });

    task = mapper.getTask(stopSource.get_token());
  }
  return stopSource;
}

void TaskCruncher::crunch(std::function<void()> f)
{
  mPTC.at("default")->enqueue(f);
}
} // namespace PB
