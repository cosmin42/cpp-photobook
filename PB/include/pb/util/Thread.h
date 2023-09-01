#pragma once

#include <condition_variable>
#include <functional>
#include <mutex>
#include <queue>
#include <thread>

#include <pb/Config.h>

namespace PB {
class Thread {
public:
  explicit Thread(
      std::stop_token stopToken, std::function<void()> finish = []() {});
  Thread(Thread const &other) { mExternalToken = other.mExternalToken; }
  virtual ~Thread() = default;

  void start();

  virtual void executeSingleTask() = 0;

protected:
  void stop();

private:
  void run();

  std::jthread    mThread;
  std::stop_token mCurrentToken;
  std::stop_token mExternalToken;

  std::function<void()> mFinish;
};
} // namespace PB