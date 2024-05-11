#pragma once

#include <boost/functional/hash.hpp>
#include <boost/uuid/uuid.hpp>

#include <thread_pool/thread_pool.h>

#include <pb/Config.h>

namespace PBDev {
class ParallelTaskConsumer {
public:
  ParallelTaskConsumer(unsigned threadCount);
  virtual ~ParallelTaskConsumer() = default;

  void abort();
  void enqueue(boost::uuids::uuid id, std::function<void()> f);

  void wait();

  [[nodiscard]] bool finished();

private:
  std::stop_source                           mSubTasksSources;
  dp::thread_pool<std::function<void(void)>> mPool;
  std::unordered_map<boost::uuids::uuid, std::future<void>,
                     boost::hash<boost::uuids::uuid>>
      mFutures;
};
} // namespace PBDev