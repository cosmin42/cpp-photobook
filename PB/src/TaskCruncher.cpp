#include <pb/TaskCruncher.h>

#include <pb/RuntimeUUID.h>

namespace PB {
void TaskCruncher::registerPTC(const std::string name, unsigned threadsCount)
{
  mPTC.emplace(name,
               std::make_unique<PBDev::ParallelTaskConsumer>(threadsCount));
}

void TaskCruncher::crunch(const std::string name, MapReducer &mapper)
{
  auto task = mapper.getNext(mStopSource.get_token());
  while (task.has_value()) {
    auto uuid = RuntimeUUID::newUUID();

    mPTC.at(name)->enqueue(uuid, [task{task}, &mapper]() {
      task->second();
      mapper.onFinished(task->first);
    });

    task = mapper.getNext(mStopSource.get_token());
  }
}

void TaskCruncher::abort() { mStopSource.request_stop(); }

} // namespace PB
