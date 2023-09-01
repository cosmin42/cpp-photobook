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
  explicit Thread(std::stop_token stopToken);
  virtual ~Thread() = default;

  void start();

  virtual void executeSingleTask() = 0;

protected:
  void stop();

private:
  void run();

  std::jthread                      mThread;
  std::stop_token                   mCurrentToken;
  std::stop_token                   mExternalToken;
  std::queue<std::function<void()>> mTasksQueue;
  std::mutex                        mTaskQueueAccess;
  std::condition_variable_any       mQueueNotEmptyCondition;
};
} // namespace PB