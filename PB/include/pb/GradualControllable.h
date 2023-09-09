#pragma once
#include <pb/Error.h>
#include <pb/Scheduable.h>

namespace PB {

class GradualControllableListener : public Scheduable {

public:
  void doFinish()
  {
    post([this]() { onFinished(); });
  }

  void doStop()
  {
    post([this]() { onStopped(); });
  }
  void doStart()
  {
    post([this]() { onStarted(); });
  }
  void doPause()
  {
    post([this]() { onPaused(); });
  }
  void doResume()
  {
    post([this]() { onResumed(); });
  }
  void doProgressUpdate()
  {
    post([this]() { onProgressUpdate(); });
  }
  void doError(Error error)
  {
    post([this, error{error}]() { onError(error); });
  }

private:
  virtual void onFinished() = 0;
  virtual void onStopped() = 0;
  virtual void onStarted() = 0;
  virtual void onPaused() = 0;
  virtual void onResumed() = 0;

  virtual void onProgressUpdate() = 0;
  virtual void onError(Error) = 0;
};
} // namespace PB