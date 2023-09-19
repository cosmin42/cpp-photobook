#pragma once

#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Storage.h>

#include <functional>
#include <memory>
#include <optional>
#include <ranges>
#include <string>

#include <pb/Error.h>

using namespace winrt;
using namespace Windows::Foundation;
using namespace Windows::Storage;

namespace PB {

class WinrtStorage final {
public:
  WinrtStorage() {}

  ~WinrtStorage() = default;

  template <template <typename, typename> typename Map>
  std::optional<Error> write(Map<std::string, std::string> const &map)
  {
    std::string rawData;

    for (auto &[key, value] : map) {
      rawData = key + "\n" + value + "\n";
    }

    hstring winData = winrt::to_hstring(rawData);

    saveDataToFileAsync(winData);
    return std::nullopt;
  }

  void setObserver(std::function<void(std::optional<Error>)> f)
  {
    mOnLoaded = f;
  }

  void load()
  {
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

    if (mOnLoaded) {
      mOnLoaded(std::nullopt);
    }
  }

  std::unordered_map<std::string, std::string> &data() { return mData; }

private:
  IAsyncAction saveDataToFileAsync(const hstring &data)
  {
    PB::printDebug("Saving data.\n");
    hstring fileName = winrt::to_hstring(Context::inst().persistentFileName());
    StorageFolder folder = ApplicationData::Current().LocalFolder();
    StorageFile   file = co_await folder.CreateFileAsync(
        fileName, CreationCollisionOption::ReplaceExisting);
    co_await FileIO::WriteTextAsync(file, data);
  }

  IAsyncOperation<hstring> loadDataFromFileAsync()
  {
    PB::printDebug("Loading data.\n");
    hstring fileName = winrt::to_hstring(Context::inst().persistentFileName());
    StorageFolder folder = ApplicationData::Current().LocalFolder();
    StorageFile   file = co_await folder.GetFileAsync(fileName);
    hstring       data = co_await FileIO::ReadTextAsync(file);
    co_return data;
  }
  std::function<void(std::optional<Error>)>    mOnLoaded;
  std::unordered_map<std::string, std::string> mData;
};
} // namespace PB