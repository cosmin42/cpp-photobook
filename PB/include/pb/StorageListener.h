#pragma once

#include <memory>
#include <string>

#include <pb/util/Traits.h>

namespace PB {

template <typename TaskManageableType, typename T>
  requires TaskManageableConcept<TaskManageableType>
class PhotoBook;

template <typename TaskManageableType, typename T>
  requires TaskManageableConcept<TaskManageableType>
class StorageListener final {
public:
  StorageListener(PhotoBook<TaskManageableType, T> &parent) : mParent(parent) {}
  ~StorageListener() = default;

  void onLoaded() { mParent.onPersistenceLoaded(); }

  void onError(Error err) { mParent.onError(err); }

private:
  PhotoBook<TaskManageableType, T> &mParent;
};
} // namespace PB