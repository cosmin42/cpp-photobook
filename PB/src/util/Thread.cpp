#include <pb/util/Thread.h>

#include <pb/Config.h>

namespace PB {
Thread::Thread(std::stop_token stopToken)
    : mExternalToken(stopToken)
{
}

void Thread::start()
{
  printDebug("Starting thread\n");
  mThread = std::jthread([this](std::stop_token token) {
    run(token);
  });
}

void Thread::stop()
{
  this->mThread.request_stop();
  printDebug("Ending thread\n");
}

void Thread::run(std::stop_token token)
{
  mCurrentToken = token;
  while (!mCurrentToken.stop_requested() && !mExternalToken.stop_requested()) {
    executeSingleTask();
  }
  if (mCurrentToken.stop_requested()) {
    finish();
  }
}

} // namespace PB