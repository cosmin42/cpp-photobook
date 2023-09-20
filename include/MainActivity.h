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

  void onProgressUpdate() {}
  void onError(PB::Error error) {}
};

class PlatformSpecificPersistence final {
public:
  PlatformSpecificPersistence() = default;
  ~PlatformSpecificPersistence() = default;

  template <template <typename, typename> typename Map>
  void write([[maybe_unused]] Map<std::string, std::string> const &map)
  {
  }

  void load() {}
};

class MainActivity final {
public:
  MainActivity() : mSettings(), mPhotoBook(mSettings, mPhotoBookListener) {}
  MainActivity(MainActivity const &) = delete;
  MainActivity(MainActivity &&) = delete;
  MainActivity &operator=(MainActivity const &) = delete;
  ~MainActivity() = default;

private:
  PB::Settings                     mSettings;
  PhotoBookListener                mPhotoBookListener;
  PB::PhotoBook<PhotoBookListener, PlatformSpecificPersistence> mPhotoBook;
};
} // namespace BL