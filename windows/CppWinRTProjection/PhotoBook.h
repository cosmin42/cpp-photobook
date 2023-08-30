#pragma once

#include "PhotoBook.g.h"

#include <pb/PhotoBook.h>

namespace winrt::CppWinRTProjection::implementation {
class GradualControllableListener final
    : public PB::GradualControllableListener {
public:
  explicit GradualControllableListener(
      const CppWinRTProjection::GradualControllableListener &parent)
      : mParent(parent)
  {
  }

  void onFinished() {}
  void onStopped() {}
  void OnStarted() {}
  void OnPaused() {}
  void OnResumed() {}

  void OnProgressUpdate() {}


private:
  CppWinRTProjection::GradualControllableListener mParent;
};

struct PhotoBook : PhotoBookT<PhotoBook> {
  explicit PhotoBook(PB::GradualControllableListener const &) {}

private:
};
} // namespace winrt::CppWinRTProjection::implementation

namespace winrt::CppWinRTProjection::factory_implementation {
struct PhotoBook : PhotoBookT<PhotoBook, implementation::PhotoBook> {};
} // namespace winrt::CppWinRTProjection::factory_implementation
