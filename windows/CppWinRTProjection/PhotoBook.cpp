#include "PhotoBook.h"
#include "PhotoBook.g.cpp"

#include <Converter.h>

namespace winrt::CppWinRTProjection::implementation {

void PhotoBook::setInputPath(const winrt::hstring inputPath)
{
  std::string inputPathNative = NativePB::Converter()(inputPath);
  mPhotoBook.setInputPath(inputPathNative);
}
void PhotoBook::setOutputPath(const winrt::hstring outputPath)
{
  std::string outputPathNative = NativePB::Converter()(outputPath);
  mPhotoBook.setInputPath(outputPathNative);
}

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

void GradualControllableListener::onError(PB::Error) {}

} // namespace winrt::CppWinRTProjection::implementation
