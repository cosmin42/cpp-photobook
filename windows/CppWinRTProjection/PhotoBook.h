#pragma once

#include "PhotoBook.g.h"

#include <pb/PhotoBook.h>

namespace winrt::CppWinRTProjection::implementation {

class GradualControllableListener;

struct PhotoBook : PhotoBookT<PhotoBook> {
  explicit PhotoBook(
      const CppWinRTProjection::GradualControllableListener &listener)
      : mPhotoBook(listener)
  {
  }

private:
  CppWinRTProjection::PhotoBook mPhotoBook;
};

class GradualControllableListener final
    : public PB::GradualControllableListener {
public:
  GradualControllableListener() = delete;
  explicit GradualControllableListener(
      const CppWinRTProjection::GradualControllableListener &parent);

  ~GradualControllableListener() = default;

  void onFinished() override;
  void onStopped() override;
  void onStarted() override;
  void onPaused() override;
  void onResumed() override;

  void onProgressUpdate() override;

private:
  CppWinRTProjection::GradualControllableListener mParent;
};
} // namespace winrt::CppWinRTProjection::implementation

namespace winrt::CppWinRTProjection::factory_implementation {
struct PhotoBook : PhotoBookT<PhotoBook, implementation::PhotoBook> {};
} // namespace winrt::CppWinRTProjection::factory_implementation
