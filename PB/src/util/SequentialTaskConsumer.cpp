#include <pb/util/SequentialTaskConsumer.h>

#include <pb/Config.h>

namespace PBDev {
SequentialTaskConsumer::SequentialTaskConsumer(std::stop_token stopToken)
    : mExternalToken(stopToken)
{
}

void SequentialTaskConsumer::start()
{
  mThread = std::jthread([this](std::stop_token token) { run(token); });
}

void SequentialTaskConsumer::stop()
{
  this->mThread.request_stop();
}

void SequentialTaskConsumer::run(std::stop_token token)
{
  PBDev::Timer timer;
  mCurrentToken = token;
  while (!mCurrentToken.stop_requested() && !mExternalToken.stop_requested()) {
    if (stoppingCondition()) {
      stop();
    }
    else {
      executeSingleTask();
    }
  }
  if (mCurrentToken.stop_requested()) {
    finish();
  }
  else
  {
    aborted();
  }
}

} // namespace PB