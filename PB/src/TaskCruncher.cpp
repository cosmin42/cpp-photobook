#include <pb/TaskCruncher.h>

#include <pb/RuntimeUUID.h>

namespace PB {
void TaskCruncher::registerPTC(const std::string name, unsigned threadsCount)
{
  mPTC.emplace(name, PBDev::ParallelTaskConsumer(threadsCount));
}

void TaskCruncher::crunch(const std::string name, MapReducer &mapper)
{
  auto task = mapper.getNext();
  while (task.has_value()) {
    auto uuid = RuntimeUUID::newUUID();

    mPTC.at(name).enqueue(uuid, [task{task}, &mapper]() {
      task->second();
      mapper.onFinished(task->first);
    });

    task = mapper.getNext();
  }
}

} // namespace PB
