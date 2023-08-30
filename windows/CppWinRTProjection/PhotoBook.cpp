#include "PhotoBook.h"
#include "PhotoBook.g.cpp"

namespace winrt::CppWinRTProjection::implementation {

GradualControllableListener::GradualControllableListener(
    CppWinRTProjection::GradualControllableListener const &parent)
    : mParent(parent)
{
}

void GradualControllableListener::onFinished() { mParent.OnFinished(); }
void GradualControllableListener::onStopped() { mParent.OnStopped(); }
void GradualControllableListener::onStarted() { mParent.OnStarted(); }
void GradualControllableListener::onPaused() { mParent.OnPaused(); }
void GradualControllableListener::onResumed() { mParent.OnResumed(); }

void GradualControllableListener::onProgressUpdate()
{
  mParent.OnProgressUpdate();
}

} // namespace winrt::CppWinRTProjection::implementation
