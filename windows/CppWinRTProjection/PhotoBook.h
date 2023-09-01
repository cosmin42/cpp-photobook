#pragma once

#include "PhotoBook.g.h"

#include <pb/PhotoBook.h>

namespace winrt::CppWinRTProjection::implementation {
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

  void onError(PB::Error) override;

private:
  CppWinRTProjection::GradualControllableListener mParent;
};

struct PhotoBook : PhotoBookT<PhotoBook> {
  explicit PhotoBook(
      const CppWinRTProjection::GradualControllableListener &listener)
      : mListener(listener), mPhotoBook(mListener)
  {
  }

  void AddMedia(const winrt::hstring inputPath);
  void setOutputPath(const winrt::hstring outputPath);

private:
  GradualControllableListener mListener;
  PB::PhotoBook               mPhotoBook;
};

} // namespace winrt::CppWinRTProjection::implementation

namespace winrt::CppWinRTProjection::factory_implementation {
struct PhotoBook : PhotoBookT<PhotoBook, implementation::PhotoBook> {};
} // namespace winrt::CppWinRTProjection::factory_implementation
