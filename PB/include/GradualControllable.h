#pragma once

namespace PB {
class GradualControllable {
public:
  void start();
  void stop();
  void pause();
  void resume();

  bool isStopped() const;
  bool isPaused() const;

private:
  virtual void doStart() = 0;
  virtual void doStop() = 0;
  virtual void doPause() = 0;
  virtual void doResume() = 0;

  bool mStopped = true;
  bool mPaused = false;

};
} // namespace PB