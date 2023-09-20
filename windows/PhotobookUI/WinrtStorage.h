#pragma once

#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Storage.h>

#include <functional>
#include <memory>
#include <optional>
#include <ranges>
#include <string>
#include <variant>

#include <pb/Config.h>
#include <pb/Error.h>

namespace PB {

class WinrtStorage final {
public:
  WinrtStorage() {}

  ~WinrtStorage() = default;

  template <template <typename, typename> typename Map>
  void write(Map<std::string, std::string> const &map)
  {
    auto dataOrError = serialize<Map>(map);
    if (std::holds_alternative<Error>(dataOrError))
    {
      if (mOnLoaded) {
        mOnLoaded(std::get<Error>(dataOrError));
      }
      return;
    }
    auto rawData = std::get<std::string>(dataOrError);
    auto winData = winrt::to_hstring(rawData);

    saveDataToFileAsync(winData);
  }

  void load() { loadDataFromFileAsync(); }

  void setObserver(std::function<void(std::optional<Error>)> f)
  {
    mOnLoaded = f;
  }

  std::unordered_map<std::string, std::string> &data() { return mData; }

private:
  std::optional<Error> parseData(std::string const &rawData)
  {
    auto                     tokensRanges = rawData | std::views::split('\n');
    std::vector<std::string> pair;
    for (const auto &tokenRange : tokensRanges) {
      auto newStr = std::string(tokenRange.begin(), tokenRange.end());
      pair.push_back(newStr);
      if (pair.size() == 2) {
        mData[pair.at(0)] = pair.at(1);
        pair.clear();
      }
    }
    if (pair.size() == 1) {
      return Error() << ErrorKind::CorruptPersistenceFile;
    }
    return std::nullopt;
  }

  template <template <typename, typename> typename Map>
  std::variant<std::string, Error>
  serialize(Map<std::string, std::string> const &map)
  {
    std::string rawData;

    for (auto &[key, value] : map) {
      if (key.find("\n") != std::string::npos ||
          value.find("\n") != std::string::npos) {
        return Error() << ErrorKind::InvalidPersistenceMap;
      }
      rawData = key + "\n" + value + "\n";
    }
    return rawData;
  }

  auto saveDataToFileAsync(const winrt::hstring data) -> winrt::fire_and_forget
  {
    PB::printDebug("Saving data.\n");
    winrt::hstring fileName =
        winrt::to_hstring(Context::inst().persistentFileName());
    winrt::Windows::Storage::StorageFolder folder =
        winrt::Windows::Storage::ApplicationData::Current().LocalFolder();
    winrt::Windows::Storage::StorageFile file = co_await folder.CreateFileAsync(
        fileName,
        winrt::Windows::Storage::CreationCollisionOption::ReplaceExisting);
    co_await winrt::Windows::Storage::FileIO::WriteTextAsync(file, data);
  }

  auto loadDataFromFileAsync() -> winrt::fire_and_forget
  {
    PB::printDebug("Loading data.\n");
    winrt::hstring fileName =
        winrt::to_hstring(Context::inst().persistentFileName());
    winrt::Windows::Storage::StorageFolder folder =
        winrt::Windows::Storage::ApplicationData::Current().LocalFolder();
    winrt::Windows::Storage::StorageFile file =
        co_await folder.GetFileAsync(fileName);
    winrt::hstring data =
        co_await winrt::Windows::Storage::FileIO::ReadTextAsync(file);

    std::string rawData = winrt::to_string(data);
    auto        maybeError = parseData(rawData);
    if (mOnLoaded) {
      mOnLoaded(maybeError);
    }
  }

  std::function<void(std::optional<Error>)>    mOnLoaded;
  std::unordered_map<std::string, std::string> mData;
};
} // namespace PB