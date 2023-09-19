#pragma once

#include <memory>
#include <string>

#include <pb/util/Traits.h>

namespace PB {

template <typename PhotoBookType, typename T>
  requires TaskManageableConcept<PhotoBookType>
class PhotoBook;

template <typename PhotoBookType, typename T>
  requires TaskManageableConcept<PhotoBookType>
class StorageListener final {
public:
  StorageListener(PhotoBook<PhotoBookType, T> &parent) : mParent(parent) {}
  ~StorageListener() = default;

  void onLoaded() { mParent.onPersistenceLoaded(); }

  void onError(Error err) { mParent.onError(err); }

private:
  PhotoBook<PhotoBookType, T> &mParent;
};
} // namespace PB