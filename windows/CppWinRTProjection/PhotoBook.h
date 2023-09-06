#pragma once

#include <memory>

#include "PhotoBook.g.h"

#include <pb/PhotoBook.h>
#include <pb/common/Log.h>

namespace winrt::CppWinRTProjection::implementation {
class GradualControllableListener final
    : public PB::GradualControllableListener {
public:
  GradualControllableListener() = delete;
  GradualControllableListener(GradualControllableListener const &) = delete;
  GradualControllableListener(GradualControllableListener &&) = delete;
  GradualControllableListener &
  operator=(GradualControllableListener const &) = delete;
  explicit GradualControllableListener(
      CppWinRTProjection::PhotoBookListener const &parent);

  ~GradualControllableListener() = default;

  void onFinished() override;
  void onStopped() override;
  void onStarted() override;
  void onPaused() override;
  void onResumed() override;

  void onProgressUpdate() override;

  void onError(PB::Error) override;

private:
  CppWinRTProjection::PhotoBookListener mParent;
};

struct PhotoBook : PhotoBookT<PhotoBook> {

  PhotoBook(CppWinRTProjection::PhotoBookListener const &listener)
      : mListener(listener), mPhotoBook(mListener)
  {
    PB::printDebug("PhotoBook constructor\n");
  }
  ~PhotoBook() = default;

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
