#pragma once
#include <pb/PhotoBook.h>

namespace BL {

class PhotoBookListener
{
public:
  void onFinished() {}
  void onStopped() {}
  void onStarted() {}
  void onPaused() {}
  void onResumed() {}

  void onProgressUpdate() {}
  void onError(PB::Error error) {}
};

class MainActivity final {
public:
  MainActivity() : mPhotoBook(mPhotoBookListener) {}
  MainActivity(MainActivity const &) = delete;
  MainActivity(MainActivity &&) = delete;
  MainActivity &operator=(MainActivity const &) = delete;
  ~MainActivity() = default;

private:
  PhotoBookListener           mPhotoBookListener;
  PB::PhotoBook<PhotoBookListener> mPhotoBook;
};
} // namespace BL