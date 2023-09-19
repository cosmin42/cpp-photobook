#pragma once

#include <functional>
#include <memory>
#include <optional>
#include <string>

#include <pb/Error.h>
#include <pb/StorageListener.h>

namespace PB {

class WinrtStorage final {
public:
  WinrtStorage() = default;
  ~WinrtStorage() = default;

  void addListener(std::shared_ptr<StorageListener> listener)
  {
    mParent = listener;
  }

  template <template <typename, typename> typename Map>
  std::optional<Error> write(Map<std::string, std::string> const &map)
  {
    return std::nullopt;
  }

  void load()
  {
    if (!mParent) {
      return;
    }
    std::unordered_map<std::string, std::string> m;
    mParent->onLoaded<std::unordered_map>(m);
  }

private:
  std::shared_ptr<StorageListener> mParent = nullptr;
};
} // namespace PB