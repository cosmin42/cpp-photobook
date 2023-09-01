#include <pb/util/Thread.h>

namespace PB {
Thread::Thread(std::stop_token stopToken) : mExternalToken(stopToken)
{
  mThread = std::jthread([this](std::stop_token token) {
    mCurrentToken = token;
    run();
  });
}

void Thread::stop() { mCurrentToken.stop_requested(); }

void Thread::run()
{
  while (!mCurrentToken.stop_requested() && !mExternalToken.stop_requested()) {
    executeSingleTask();
  }
}

} // namespace PB