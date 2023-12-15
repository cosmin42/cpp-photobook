#pragma once

#include <thread_pool/thread_pool.h>

#include <pb/Config.h>

namespace PB {
class ParallelTaskConsumer {
public:
  ParallelTaskConsumer();
  virtual ~ParallelTaskConsumer() = default;

  void abort();
  void enqueue(std::function<void()> f);

  [[nodiscard]] bool finished();

private:
  std::stop_source                           mSubTasksSources;
  dp::thread_pool<std::function<void(void)>> mPool;
  std::vector<std::future<void>>             mFutures;
};
} // namespace PB