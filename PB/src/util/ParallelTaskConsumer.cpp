#include <pb/tasks/ParallelTaskConsumer.h>

namespace PBDev {
ParallelTaskConsumer::ParallelTaskConsumer(unsigned threadCount)
    : mPool(threadCount)
{
}

void ParallelTaskConsumer::abort() { mSubTasksSources.request_stop(); }

[[nodiscard]] bool ParallelTaskConsumer::finished()
{
  for (auto &future : mFutures) {
    bool isFinished = (future.second.wait_for(std::chrono::seconds(0)) ==
                       std::future_status::ready);
    if (!isFinished) {
      return false;
    }
  }
  return true;
}

void ParallelTaskConsumer::enqueue(ParallelTaskConsumerId id,
                                   std::function<void()> f)
{
  std::future<void> token = mPool.enqueue(f);
  mFutures[id] = std::move(token);
}

void ParallelTaskConsumer::wait()
{
  for (auto &[path, future] : mFutures) {
    PBDev::Unused(path);
    future.wait();
  }
}

} // namespace PBDev