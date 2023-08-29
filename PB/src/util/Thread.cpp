#include <util/Thread.h>
namespace PB {
Thread::Thread(std::stop_token stopToken) : mExternalToken(stopToken)
{
  mThread = std::jthread([this](std::stop_token token) {
    mCurrentToken = token;
    run();
  });
}

void Thread::run()
{
  while (!mCurrentToken.stop_requested() && !mExternalToken.stop_requested()) {
    runNextTask();
  }
}

void Thread::post(std::function<void()> f)
{
  std::scoped_lock exclusiveAccess(mTaskQueueAccess);
  mTasksQueue.push(f);
  mQueueNotEmptyCondition.notify_one();
}

void Thread::runNextTask() { getNextTask()(); }

auto Thread::getNextTask() -> std::function<void()>
{
  std::unique_lock exclusiveAccess(mTaskQueueAccess);
  bool             interrupted = mQueueNotEmptyCondition.wait(
      exclusiveAccess, mExternalToken,
      [&mTasksQueue = mTasksQueue]() { return !mTasksQueue.empty(); });
  if (interrupted) {
    return []() {};
  }
  auto f = mTasksQueue.front();
  mTasksQueue.pop();
  return f;
}
} // namespace PB