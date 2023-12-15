#include <PB/util/ParallelTaskConsumer.h>

namespace PB {
ParallelTaskConsumer::ParallelTaskConsumer()
    : mPool(Context::THUMBNAIL_THREADPOOL_THREAD_COUNT)
{
}
void ParallelTaskConsumer::abort() { mSubTasksSources.request_stop(); }

[[nodiscard]] bool ParallelTaskConsumer::finished()
{
  for (auto &future : mFutures) {
    bool isFinished =
        (future.wait_for(std::chrono::seconds(0)) == std::future_status::ready);
    if (!isFinished) {
      return false;
    }
  }
  return true;
}

void ParallelTaskConsumer::enqueue(std::function<void()> f)
{
  std::future<void> token = mPool.enqueue(f);
  mFutures.push_back(std::move(token));
}
} // namespace PB