#pragma once

#include <string>

#include <Error.h>

namespace PB {
class PhotoBookListener {
  virtual void onFinished() = 0;
  virtual void onStarted() = 0;
  virtual void onProgressUpdate() = 0;
  virtual void onError(Error error) = 0;
};

class PhotoBook final {
public:
  explicit PhotoBook(PhotoBookListener &listener);
  PhotoBook() = delete;
  PhotoBook(PhotoBook const &) = delete;
  PhotoBook(PhotoBook &&) = delete;
  PhotoBook &operator=(PhotoBook const &) = delete;
  ~PhotoBook() = default;

  void loadImages([[maybe_unused]] std::string const &root);

  void exportIntermediaryImages([[maybe_unused]] std::string const &path);

private:
  PhotoBookListener &mListener;
};
} // namespace PB