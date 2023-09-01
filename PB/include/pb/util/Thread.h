#pragma once

#include <functional>
#include <queue>
#include <stop_token>
#include <thread>

namespace PB {
class Thread {
public:
  explicit Thread(
      std::stop_token stopToken, std::function<void()> finish = []() {});
  Thread(Thread const &other) { mExternalToken = other.mExternalToken; }
  Thread(Thread &&other) noexcept { mExternalToken = other.mExternalToken; }
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