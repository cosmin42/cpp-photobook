#pragma once

#include <functional>
#include <queue>
#include <stop_token>
#include <thread>

namespace PB {
class Thread {
public:
  explicit Thread(std::stop_token stopToken);

  Thread(Thread const &other)
  {
    mExternalToken = other.mExternalToken;
    mFinish = other.mFinish;
  }
  Thread(Thread &&other) noexcept
  {
    mExternalToken = other.mExternalToken;
    mFinish = other.mFinish;
  }
  virtual ~Thread() = default;

  void start();

  virtual void executeSingleTask() = 0;

protected:
  void stop();

  std::function<void()> mFinish = nullptr;

private:
  void run();

  std::stop_token mCurrentToken;
  std::stop_token mExternalToken;
  std::jthread    mThread;
};
} // namespace PB