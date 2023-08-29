#pragma once

#include <condition_variable>
#include <functional>
#include <mutex>
#include <queue>
#include <thread>

#include <Config.h>

namespace PB {
class Thread final {
public:
  explicit Thread(std::stop_token stopToken);
  ~Thread() = default;

  void post(std::function<void()> f);

private:
  void run();

  void runNextTask();

  auto getNextTask() -> std::function<void()>;

  std::jthread                      mThread;
  std::stop_token                   mCurrentToken;
  std::stop_token                   mExternalToken;
  std::queue<std::function<void()>> mTasksQueue;
  std::mutex                        mTaskQueueAccess;
  std::condition_variable_any       mQueueNotEmptyCondition;
};
} // namespace PB