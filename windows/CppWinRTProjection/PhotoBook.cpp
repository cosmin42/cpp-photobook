#include "PhotoBook.h"
#include "PhotoBook.g.cpp"

#include <string>

#include <Converter.h>

namespace winrt::CppWinRTProjection::implementation {

void PhotoBook::AddMedia(const winrt::hstring mediaPath)
{
  std::string inputPathNative = NativePB::Converter()(mediaPath);
  mPhotoBook->addMedia(inputPathNative);
}
void PhotoBook::setOutputPath(const winrt::hstring outputPath)
{
  std::string outputPathNative = NativePB::Converter()(outputPath);
  //mPhotoBook.setOutputPath(outputPathNative);
}

GradualControllableListener::GradualControllableListener(
    CppWinRTProjection::PhotoBookListener const &parent)
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
