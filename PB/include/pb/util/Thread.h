#pragma once

#include <functional>
#include <queue>
#include <stop_token>
#include <thread>

namespace PB {
class Thread {
public:
  explicit Thread(std::stop_token stopToken);

  Thread(Thread const &other) { mExternalToken = other.mExternalToken; }
  Thread(Thread &&other) noexcept { mExternalToken = other.mExternalToken; }
  Thread &operator=(Thread const &) = delete;
  virtual ~Thread() = default;

  void start();

  virtual void executeSingleTask() = 0;

  virtual void finish() = 0;

protected:
  void stop();

private:
  void run(std::stop_token token);

  std::stop_token mCurrentToken;
  std::stop_token mExternalToken;
  std::jthread    mThread;
};
} // namespace PB