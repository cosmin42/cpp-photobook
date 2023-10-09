#pragma once

#include <pb/Error.h>
#include <pb/util/Concepts.h>

namespace PB {
template <typename PhotoBookType, typename PersistenceType>
  requires PhotoBookListenerConcept<PhotoBookType>
class PhotoBook;

template <typename PhotoBookType, typename PersistenceType>
  requires PhotoBookListenerConcept<PhotoBookType>
class GalleryListener final {
public:
  GalleryListener(PhotoBook<PhotoBookType, PersistenceType> &parent)
      : mParent(parent)
  {
  }
  ~GalleryListener() = default;

private:
  PhotoBook<PhotoBookType, PersistenceType> &mParent;
};
} // namespace PB