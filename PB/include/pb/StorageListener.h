#pragma once

#include <memory>
#include <string>

#include <pb/util/Traits.h>

namespace PB {

template <typename TaskManageableType>
  requires TaskManageableConcept<TaskManageableType>
class PhotoBook;

template <typename TaskManageableType>
  requires TaskManageableConcept<TaskManageableType>
class StorageListener final {
public:
  StorageListener(PhotoBook<TaskManageableType> &parent) : mParent(parent) {}
  ~StorageListener() = default;

  void onLoaded() { mParent.onPersistenceLoaded(); }

  void onError(Error err) { mParent.onError(err); }

private:
  PhotoBook<TaskManageableType> &mParent;
};
} // namespace PB