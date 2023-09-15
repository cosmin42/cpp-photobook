#pragma once

#include <pb/GalleryListener.h>

namespace PB {

template <typename T> class Gallery final {
public:
  Gallery(GalleryListener<T> &listener) : mListener(listener) {}
  ~Gallery() = default;

private:
  GalleryListener<T> &mListener;
};
} // namespace PB