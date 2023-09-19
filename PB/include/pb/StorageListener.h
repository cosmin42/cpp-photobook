#pragma once

#include <string>

namespace PB {
class StorageListener final {
public:
  StorageListener() = default;
  ~StorageListener() = default;

  template <template <typename, typename> typename Map>
  void onLoaded(Map<std::string, std::string> map)
  {
  }

private:
};
} // namespace PB