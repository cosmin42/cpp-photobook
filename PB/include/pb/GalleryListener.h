#pragma once

#include <pb/Error.h>
#include <pb/util/Concepts.h>

namespace PB {
template <typename TaskManageableType>
  requires TaskManageableConcept<TaskManageableType>
class PhotoBook;

template <typename T>
  requires TaskManageableConcept<T>
class GalleryListener final {
public:
  GalleryListener(PhotoBook<T> &parent) : mParent(parent) {}
  ~GalleryListener() = default;

private:
  PhotoBook<T> &mParent;
};
} // namespace PB