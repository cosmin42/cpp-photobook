#include <GradualControllable.h>

namespace PB {
void GradualControllable::start()
{
  mStopped = false;
  doStart();
}
void GradualControllable::stop()
{
  doStop();
  mStopped = true;
}
void GradualControllable::pause()
{
  doPause();
  mPaused = true;
}
void GradualControllable::resume()
{
  mPaused = false;
  doResume();
}

bool GradualControllable::isStopped() const { return mStopped; }
bool GradualControllable::isPaused() const { return mPaused; }
} // namespace PB