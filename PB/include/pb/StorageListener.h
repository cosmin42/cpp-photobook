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

  template <template <typename, typename> typename Map>
  void onLoaded(Map<std::string, std::string> &map)
  {
  }

  void onError(Error err) {}

private:
  PhotoBook<TaskManageableType> &mParent;
};
} // namespace PB