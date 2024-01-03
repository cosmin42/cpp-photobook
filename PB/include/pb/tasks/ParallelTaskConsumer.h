#pragma once

#include <thread_pool/thread_pool.h>

#include <pb/Config.h>

namespace PBDev {
class ParallelTaskConsumer {
public:
  ParallelTaskConsumer();
  virtual ~ParallelTaskConsumer() = default;

  void abort();
  void enqueue(Path root, std::function<void()> f);

  void wait();

  [[nodiscard]] bool finished();

private:
  std::stop_source                                   mSubTasksSources;
  dp::thread_pool<std::function<void(void)>>         mPool;
  std::unordered_map<Path, std::future<void>> mFutures;
};
} // namespace PBDev