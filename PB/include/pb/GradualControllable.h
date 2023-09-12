#pragma once
#include <pb/Error.h>
#include <pb/Scheduable.h>

namespace PB {

class GradualControllableListener : public Scheduable {

public:
  void doFinish();
  void doStop();
  void doStart();
  void doPause();
  void doResume();
  void doProgressUpdate();
  void doError(Error error);

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