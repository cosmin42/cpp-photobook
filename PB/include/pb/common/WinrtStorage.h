#pragma once

#include <functional>
#include <memory>
#include <optional>
#include <string>

#include <pb/Error.h>

namespace PB {

class WinrtStorage final {
public:
  WinrtStorage() = default;

  ~WinrtStorage() = default;

  template <template <typename, typename> typename Map>
  std::optional<Error> write(Map<std::string, std::string> const &map)
  {
    return std::nullopt;
  }

  void setObserver(std::function<void(std::optional<Error>)> f)
  {
    mOnLoaded = f;
  }

  void load()
  {
    if (mOnLoaded) {
      mOnLoaded(std::nullopt);
    }
  }

  std::unordered_map<std::string, std::string> &data() { return mData; }

private:
  std::function<void(std::optional<Error>)>    mOnLoaded;
  std::unordered_map<std::string, std::string> mData;
};
} // namespace PB