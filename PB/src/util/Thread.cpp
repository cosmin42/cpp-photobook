#include <pb/util/Thread.h>

#include <pb/Config.h>

namespace PB {
Thread::Thread(std::stop_token stopToken, std::function<void()> onFinish)
    : mExternalToken(stopToken), mFinish(onFinish)
{
}

void Thread::start()
{
  mThread = std::jthread([this](std::stop_token token) {
    mCurrentToken = token;
    run();
  });
}

void Thread::stop() { this->mThread.request_stop();}

void Thread::run()
{
  while (!mCurrentToken.stop_requested() && !mExternalToken.stop_requested()) {
    executeSingleTask();
  }
  
}

} // namespace PB