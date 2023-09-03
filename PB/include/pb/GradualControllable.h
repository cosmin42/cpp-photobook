#pragma once
#include <pb/Error.h>

namespace PB {

class GradualControllableListener {

public:
  virtual void onFinished() = 0;
  virtual void onStopped() = 0;
  virtual void onStarted() = 0;
  virtual void onPaused() = 0;
  virtual void onResumed() = 0;

  virtual void onProgressUpdate() = 0;
  virtual void onError(Error error) = 0;
};
} // namespace PB