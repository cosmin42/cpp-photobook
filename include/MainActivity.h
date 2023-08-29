#pragma once
#include <PhotoBook.h>

namespace BL {
class MainActivity final : public PB::PhotoBookListener {
public:
  MainActivity() : mPhotoBook(*this) {}
  MainActivity(MainActivity const &) = delete;
  MainActivity(MainActivity &&) = delete;
  MainActivity &operator=(MainActivity const &) = delete;
  ~MainActivity() = default;

  void onFinished() override {}
  void onStarted() override {}
  void onProgressUpdate() override {}
  void onError(PB::Error error) override {}

private:
  PB::PhotoBook mPhotoBook;
};
} // namespace BL