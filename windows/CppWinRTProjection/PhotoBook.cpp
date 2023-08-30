#include "PhotoBook.h"
#include "PhotoBook.g.cpp"

namespace winrt::CppWinRTProjection::implementation {

GradualControllableListener::GradualControllableListener(
    CppWinRTProjection::GradualControllableListener const &parent)
    : mParent(parent)
{
}

void GradualControllableListener::onFinished() {}

void GradualControllableListener::onStopped() {}
void GradualControllableListener::onStarted() {}
void GradualControllableListener::onPaused() {}
void GradualControllableListener::onResumed() {}

void GradualControllableListener::onProgressUpdate() {}

} // namespace winrt::CppWinRTProjection::implementation
