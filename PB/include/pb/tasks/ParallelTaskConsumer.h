#pragma once

#include <boost/functional/hash.hpp>
#include <boost/uuid/uuid.hpp>

#include <thread_pool/thread_pool.h>

#include <pb/Config.h>
#include <pb/util/Traits.h>

namespace PBDev {
class ParallelTaskConsumer {
public:
  ParallelTaskConsumer(unsigned threadCount);
  virtual ~ParallelTaskConsumer() = default;

  void abort();
  void enqueue(std::function<void()> f);

  void wait();

  [[nodiscard]] bool finished();

private:
  dp::thread_pool<std::function<void(void)>> mPool;
  std::unordered_map<ParallelTaskConsumerId, std::future<void>,
                     boost::hash<ParallelTaskConsumerId>>
      mFutures;
};
} // namespace PBDev