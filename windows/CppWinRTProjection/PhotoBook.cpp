#include "PhotoBook.h"
#include "PhotoBook.g.cpp"

namespace winrt::CppWinRTProjection::implementation {

void PhotoBook::setInputPath(const winrt::hstring inputPath) {}
void PhotoBook::setOutputPath(const winrt::hstring outputPath) {}

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

void GradualControllableListener::onError(PB::Error) {

}

} // namespace winrt::CppWinRTProjection::implementation
