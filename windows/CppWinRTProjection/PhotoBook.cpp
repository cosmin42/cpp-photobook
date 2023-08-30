#include "PhotoBook.h"
#include "PhotoBook.g.cpp"

namespace winrt::CppWinRTProjection::implementation {

    GradualControllableListener::GradualControllableListener(
    CppWinRTProjection::GradualControllableListener const &parent)
    : mParent(parent)
{
}

void GradualControllableListener::onFinished() {}

} // namespace winrt::CppWinRTProjection::implementation
