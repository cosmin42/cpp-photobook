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

  void onStagedImageAdded(PB::Path path){}

  void post(std::function<void()>) {}

  void onProgressUpdate(int, int) {}
  void onError(PB::Error error) {}
};

class PlatformSpecificPersistence final {
public:
  PlatformSpecificPersistence() = default;
  ~PlatformSpecificPersistence() = default;

  static std::string localFolder() { return "."; }

  template <template <typename, typename> typename Map>
  void write(Map<std::string, std::string> const &map)
  {
  }

  void load(std::function<void(std::optional<PB::Error>)>) {}

  void setObserver(std::function<void(std::optional<PB::Error>)> f) {}

  std::unordered_map<std::string, std::string> &data() { return mData; }

private:
  std::unordered_map<std::string, std::string> mData;
};

class MainActivity final {
public:
  MainActivity() : mPhotoBook(mPhotoBookListener) {}
  MainActivity(MainActivity const &) = delete;
  MainActivity(MainActivity &&) = delete;
  MainActivity &operator=(MainActivity const &) = delete;
  ~MainActivity() = default;

private:
  PhotoBookListener mPhotoBookListener;
  PB::PhotoBook<PhotoBookListener, PlatformSpecificPersistence> mPhotoBook;
};
} // namespace BL