#pragma once

#include <pb/PhotoBook.h>

namespace BL {

class PhotoBookListener final {
public:
  void onFinished() {}
  void onStopped() {}
  void onStarted() {}
  void onPaused() {}
  void onResumed() {}

  void onUnstagedImageAdded(PB::Path fullPath, PB::Path mediumPath,
                            PB::Path smallPath, int position)
  {
  }

  void post(std::function<void()>) {}

  void onProgressUpdate(int, int) {}
  void onError(PB::Error error) {}

  void onAddingUnstagedImagePlaceholder(unsigned size) {}

  void onMappingFinished(PB::Path) {}
};

class MainActivity final {
public:
  MainActivity() : mPhotoBook(mPhotoBookListener, PB::Path("."), {0, 0}) {}
  MainActivity(MainActivity const &) = delete;
  MainActivity(MainActivity &&) = delete;
  MainActivity &operator=(MainActivity const &) = delete;
  ~MainActivity() = default;

private:
  PhotoBookListener mPhotoBookListener;
  PB::PhotoBook<PhotoBookListener> mPhotoBook;
};
} // namespace BL