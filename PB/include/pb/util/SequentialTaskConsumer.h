#pragma once

#include <stop_token>
#include <thread>

namespace PBDev {
class SequentialTaskConsumer {
public:
  explicit SequentialTaskConsumer(std::stop_token stopToken);

  SequentialTaskConsumer(SequentialTaskConsumer const &other)
  {
    mExternalToken = other.mExternalToken;
  }
  SequentialTaskConsumer(SequentialTaskConsumer &&other) noexcept
  {
    mExternalToken = other.mExternalToken;
  }
  SequentialTaskConsumer &operator=(SequentialTaskConsumer const &) = delete;
  virtual ~SequentialTaskConsumer() = default;

  void start();

  virtual void executeSingleTask() = 0;

  virtual void finish() = 0;
  virtual void aborted() = 0;

  virtual bool stoppingCondition() = 0;

protected:
  // TODO: Rename to abort()
  void stop();

private:
  void run(std::stop_token token);

  std::stop_token mCurrentToken;
  std::stop_token mExternalToken;
  std::jthread    mThread;
};
} // namespace PB