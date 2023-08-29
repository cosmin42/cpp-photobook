#pragma once
#include <Error.h>

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

class GradualControllable {
public:
  explicit GradualControllable(GradualControllableListener &listener)
      : mListener(listener)
  {
  }
  virtual ~GradualControllable() = default;

  void start();
  void stop();
  void pause();
  void resume();
  void finish();

  bool isStopped() const;
  bool isPaused() const;

protected:
  GradualControllableListener &mListener;

private:
  virtual void doStart() = 0;
  virtual void doStop() = 0;
  virtual void doPause() = 0;
  virtual void doResume() = 0;
  virtual void doFinish() = 0;

  void wait();

  bool mStopped = true;
  bool mPaused = false;
};
} // namespace PB