#pragma once

#include <pb/Error.h>
#include <pb/util/Concepts.h>

namespace PB {
template <typename TaskManageableType, typename T>
  requires TaskManageableConcept<TaskManageableType>
class PhotoBook;

template <typename TaskManageableType, typename T>
  requires TaskManageableConcept<TaskManageableType>
class GalleryListener final {
public:
  GalleryListener(PhotoBook<TaskManageableType, T> &parent) : mParent(parent) {}
  ~GalleryListener() = default;

private:
  PhotoBook<TaskManageableType, T> &mParent;
};
} // namespace PB