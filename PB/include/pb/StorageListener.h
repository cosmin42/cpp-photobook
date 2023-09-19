#pragma once

#include <memory>
#include <string>

#include <pb/util/Traits.h>

namespace PB {

template <typename PhotoBookType, typename PersistenceType>
  requires PhotoBookConcept<PhotoBookType>
class PhotoBook;

template <typename PhotoBookType, typename PersistenceType>
  requires PhotoBookConcept<PhotoBookType>
class StorageListener final {
public:
  StorageListener(PhotoBook<PhotoBookType, PersistenceType> &parent)
      : mParent(parent)
  {
  }
  ~StorageListener() = default;

  void onLoaded() { mParent.onPersistenceLoaded(); }

  void onError(Error err) { mParent.onError(err); }

private:
  PhotoBook<PhotoBookType, PersistenceType> &mParent;
};
} // namespace PB