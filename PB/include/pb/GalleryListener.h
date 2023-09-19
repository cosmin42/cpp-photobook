#pragma once

#include <pb/Error.h>
#include <pb/util/Concepts.h>

namespace PB {
template <typename PhotoBookType, typename T>
  requires PhotoBookConcept<PhotoBookType>
class PhotoBook;

template <typename PhotoBookType, typename T>
  requires PhotoBookConcept<PhotoBookType>
class GalleryListener final {
public:
  GalleryListener(PhotoBook<PhotoBookType, T> &parent) : mParent(parent) {}
  ~GalleryListener() = default;

private:
  PhotoBook<PhotoBookType, T> &mParent;
};
} // namespace PB