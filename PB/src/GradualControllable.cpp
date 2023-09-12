#include <pb/GradualControllable.h>

namespace PB {
void GradualControllableListener::doFinish()
{
  post([this]() { onFinished(); });
}

void GradualControllableListener::doStop()
{
  post([this]() { onStopped(); });
}
void GradualControllableListener::doStart()
{
  post([this]() { onStarted(); });
}
void GradualControllableListener::doPause()
{
  post([this]() { onPaused(); });
}
void GradualControllableListener::doResume()
{
  post([this]() { onResumed(); });
}
void GradualControllableListener::doProgressUpdate()
{
  post([this]() { onProgressUpdate(); });
}
void GradualControllableListener::doError(Error error)
{
  post([this, error{error}]() { onError(error); });
}
} // namespace PB