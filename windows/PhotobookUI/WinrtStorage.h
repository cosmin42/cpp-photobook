#pragma once

#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Storage.h>

#include <functional>
#include <memory>
#include <optional>
#include <ranges>
#include <string>

#include <pb/Config.h>
#include <pb/Error.h>

namespace PB {

class WinrtStorage final {
public:
  WinrtStorage() {}

  ~WinrtStorage() = default;

  template <template <typename, typename> typename Map>
  std::optional<Error> write([[maybe_unused]] Map<std::string, std::string> const &map)
  {
    /*
    std::string rawData;

    for (auto &[key, value] : map) {
      rawData = key + "\n" + value + "\n";
    }

    hstring winData = winrt::to_hstring(rawData);

    saveDataToFileAsync(winData);
    */
    return std::nullopt;
  }

  void setObserver(std::function<void(std::optional<Error>)> f)
  {
    mOnLoaded = f;
  }

  void load()
  {
    /*
    auto        winData = loadDataFromFileAsync();
    std::string rawData = winrt::to_string(winData.as<winrt::hstring>());

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
    */
    if (mOnLoaded) {
      mOnLoaded(std::nullopt);
    }
  }

  void onDataLoaded([[maybe_unused]] winrt::hstring winData)
  {

  }

  std::unordered_map<std::string, std::string> &data() { return mData; }

private:
  auto saveDataToFileAsync(const winrt::hstring &data) -> winrt::fire_and_forget
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
  }

  std::function<void(std::optional<Error>)>    mOnLoaded;
  std::unordered_map<std::string, std::string> mData;
};
} // namespace PB