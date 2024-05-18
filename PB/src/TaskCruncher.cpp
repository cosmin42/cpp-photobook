#include <pb/TaskCruncher.h>

#include <pb/RuntimeUUID.h>

namespace PB {
void TaskCruncher::registerPTC(const std::string poolName,
                               unsigned          threadsCount)
{
  mPTC.emplace(poolName,
               std::make_unique<PBDev::ParallelTaskConsumer>(threadsCount));
}

void TaskCruncher::crunch(const std::string poolName, MapReducer &mapper)
{
  auto task = mapper.getNext(mStopSource.get_token());
  while (task.has_value()) {

    mPTC.at(poolName)->enqueue([task{task}, &mapper]() {
      task->second();
      mapper.onFinished(task->first);
    });

    task = mapper.getNext(mStopSource.get_token());
  }
}

void TaskCruncher::abort() { mStopSource.request_stop(); }

} // namespace PB
