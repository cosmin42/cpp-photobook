#pragma once

namespace PB {
class PhotoBookListener {
  virtual void onFinished() = 0;
  virtual void onStarted() = 0;
  virtual void onProgressUpdate() = 0;
};

class PhotoBook final {
public:
  explicit PhotoBook(PhotoBookListener &listener);
  PhotoBook() = delete;
  PhotoBook(PhotoBook const &) = delete;
  PhotoBook(PhotoBook &&) = delete;
  PhotoBook &operator=(PhotoBook const &) = delete;
  ~PhotoBook() = default;

private:
  PhotoBookListener &mListener;
};
} // namespace PB