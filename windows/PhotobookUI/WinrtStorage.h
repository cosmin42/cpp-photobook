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

using namespace winrt::Windows::Storage;

class WinrtStorage final {
public:
  static std::string localFolder()
  {
    winrt::Windows::Storage::StorageFolder folder =
        ApplicationData::Current().LocalFolder();

    return winrt::to_string(folder.Path());
  }

  template <template <typename, typename> typename Map>
  void write(Path directory, Path fileName,
             Map<std::string, std::string> const      &map,
             std::function<void(std::optional<Error>)> onFinish)
  {
    auto dataOrError = serialize<Map>(map);
    if (std::holds_alternative<Error>(dataOrError)) {
      if (onFinish) {
        onFinish(std::get<Error>(dataOrError));
      }
      return;
    }
    auto rawData = std::get<std::string>(dataOrError);
    auto winData = winrt::to_hstring(rawData);

    winrt::hstring directoryWinStr = winrt::to_hstring(directory.string());
    winrt::hstring fileNameWinStr = winrt::to_hstring(fileName.string());

    saveDataToFileAsync(directoryWinStr, fileNameWinStr, winData);
    onFinish(std::nullopt);
  }

  template <template <typename, typename> typename Map>
  void write(std::string const &directory, std::string const &fileName,
             Map<std::string, std::string> const      &map,
             std::function<void(std::optional<Error>)> onFinish)
  {
    write<Map>(Path(directory), Path(fileName), map, onFinish);
  }

  template <template <typename, typename> typename Map>
  void write(Map<std::string, std::string> const      &map,
             std::function<void(std::optional<Error>)> onFinish)
  {
    write<Map>(localFolder(), Context::PERSISTENCE_FILENAME, map, onFinish);
  }

  void load(std::function<void(std::optional<Error>)> onFinished)
  {
    loadDataFromFileAsync(onFinished);
  }

  std::unordered_map<std::string, std::string> &data() { return mData; }

  std::variant<std::unordered_map<std::string, std::string>, Error>
  parseData(std::string const &rawData)
  {
    std::unordered_map<std::string, std::string> parsed;
    auto                     tokensRanges = rawData | std::views::split('\n');
    std::vector<std::string> pair;
    for (const auto &tokenRange : tokensRanges) {
      auto newStr = std::string(tokenRange.begin(), tokenRange.end());
      if (newStr.empty()) {
        continue;
      }
      pair.push_back(newStr);
      if (pair.size() == 2) {
        parsed[pair.at(0)] = pair.at(1);
        pair.clear();
      }
    }
    if (pair.size() == 1) {
      return Error() << ErrorCode::CorruptPersistenceFile;
    }
    return parsed;
  }

private:
  template <template <typename, typename> typename Map>
  std::variant<std::string, Error>
  serialize(Map<std::string, std::string> const &map)
  {
    std::string rawData;

    for (auto &[key, value] : map) {
      if (key.find("\n") != std::string::npos ||
          value.find("\n") != std::string::npos) {
        return Error() << ErrorCode::InvalidPersistenceMap;
      }
      rawData = rawData + key + "\n" + value + "\n";
    }
    return rawData;
  }

  auto saveDataToFileAsync(const winrt::hstring directory,
                           const winrt::hstring fileName,
                           const winrt::hstring data) -> winrt::fire_and_forget
  {
    PB::printDebug("Saving data.\n");
    StorageFolder folder =
        co_await StorageFolder::GetFolderFromPathAsync(directory);
    StorageFile file = co_await folder.CreateFileAsync(
        fileName, CreationCollisionOption::ReplaceExisting);
    co_await FileIO::WriteTextAsync(file, data);
  }

  auto
  loadDataFromFileAsync(std::function<void(std::optional<Error>)> onFinished)
      -> winrt::fire_and_forget
  {
    PB::printDebug("Loading data.\n");
    winrt::hstring fileName = winrt::to_hstring(Context::PERSISTENCE_FILENAME);
    StorageFolder  folder = ApplicationData::Current().LocalFolder();

    auto result = co_await folder.TryGetItemAsync(fileName);

    if (!result) {
      StorageFile file = co_await folder.CreateFileAsync(
          fileName, CreationCollisionOption::ReplaceExisting);
    }
    StorageFile    file = co_await folder.GetFileAsync(fileName);
    winrt::hstring data = co_await FileIO::ReadTextAsync(file);

    std::string rawData = winrt::to_string(data);
    auto        responseOrError = parseData(rawData);
    if (onFinished) {
      if (std::holds_alternative<Error>(responseOrError)) {
        onFinished(std::get<Error>(responseOrError));
      }
      else {
        auto &newData = std::get<std::unordered_map<std::string, std::string>>(
            responseOrError);
        mData.insert(newData.begin(), newData.end());
        onFinished(std::nullopt);
      }
    }
  }

  std::unordered_map<std::string, std::string> mData;
};
} // namespace PB