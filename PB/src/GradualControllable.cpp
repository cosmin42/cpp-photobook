#include <GradualControllable.h>
#include <thread>

namespace PB {
void GradualControllable::start()
{
  if (!mStopped) {
    return;
  }

  mStopped = false;
  doStart();
}
void GradualControllable::stop()
{
  doStop();
  mListener.onStopped();
  mStopped = true;
}

void GradualControllable::finish()
{
  doFinish();
  mListener.onFinished();
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

void GradualControllable::wait() {  }
} // namespace PB