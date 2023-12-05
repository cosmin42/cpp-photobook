#include <pb/util/Thread.h>

#include <pb/Config.h>

namespace PB {
SequentialTaskConsumer::SequentialTaskConsumer(std::stop_token stopToken)
    : mExternalToken(stopToken)
{
}

void SequentialTaskConsumer::start()
{
  printDebug("Starting thread\n");
  mThread = std::jthread([this](std::stop_token token) { run(token); });
}

void SequentialTaskConsumer::stop()
{
  this->mThread.request_stop();
  printDebug("Ending thread\n");
}

void SequentialTaskConsumer::run(std::stop_token token)
{
  Timer timer;
  mCurrentToken = token;
  while (!mCurrentToken.stop_requested() && !mExternalToken.stop_requested()) {
    executeSingleTask();
  }
  if (mCurrentToken.stop_requested()) {
    printDebug("Thread Elapsed time %f\n", timer.elapsed());
    finish();
  }
}

} // namespace PB