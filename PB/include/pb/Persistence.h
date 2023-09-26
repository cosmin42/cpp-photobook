#pragma once

#include <fstream>
#include <functional>
#include <memory>
#include <optional>
#include <ranges>
#include <string>
#include <unordered_map>
#include <variant>

#include <pb/Error.h>
#include <pb/util/Traits.h>

namespace PB {

template <typename PersistenceType = void> class Persistence final {
public:
  Persistence() = default;
  Persistence(Persistence const &) = delete;
  Persistence(Persistence &&) noexcept = delete;
  Persistence &operator=(Persistence const &) = delete;
  ~Persistence() = default;

  static std::string localFolder() { return PersistenceType::localFolder(); }

  void write(std::function<void(std::optional<Error>)> f)
  {
    mPersistence.template write<std::unordered_map>(
        mCache, [f{f}](std::optional<Error> out) { f(out); });
  }

  void write(std::string directory, std::string fileName,
             std::function<void(std::optional<Error>)> f)
  {
    mPersistence.template write<std::unordered_map>(
        directory, fileName, mCache,
        [f{f}](std::optional<Error> out) { f(out); });
  }

  void write(Path directory, Path fileName,
             std::function<void(std::optional<Error>)> f)
  {
    mPersistence.template write<std::unordered_map>(
        directory, fileName, mCache,
        [f{f}](std::optional<Error> out) { f(out); });
  }

  void load(std::function<void(std::optional<Error>)> f)
  {
    mPersistence.load([f{f}, this](std::optional<Error> out) {
      if (!out) {
        mCache.clear();
        auto &cacheRef = mPersistence.data();
        mCache.insert(cacheRef.begin(), cacheRef.end());
        cacheRef.clear();
      }
      f(out);
    });
  }

  void
  load(Path path,
       std::function<void(
           std::variant<std::unordered_map<std::string, std::string>, Error>)>
           f)
  {
    std::ifstream     t(path);
    std::stringstream buffer;
    buffer << t.rdbuf();

    auto parsedMapOrError = mPersistence.parseData(buffer.str());
    f(parsedMapOrError);
  }

  std::unordered_map<std::string, std::string> &cache() { return mCache; }

private:
  std::unordered_map<std::string, std::string> mCache;
  PersistenceType                              mPersistence;
};

template <> class Persistence<void> final {
public:
  Persistence() = default;
  explicit Persistence(Path const &path) : mLocalFile(path) {}
  Persistence(Persistence const &) = delete;
  Persistence(Persistence &&) noexcept = delete;
  Persistence &operator=(Persistence const &) = delete;
  ~Persistence() = default;

  Persistence &setLocalFolder(Path const &path)
  {
    mLocalFile = path;
    return *this;
  }

  void write(std::function<void(std::optional<Error>)> f)
  {
    write(mLocalFile, f);
  }

  void write(std::string path, std::function<void(std::optional<Error>)> f)
  {
    write(Path(path), f);
  }

  void write(Path path, std::function<void(std::optional<Error>)> f)
  {
    std::ofstream ofs(path.string());

    if (!ofs.is_open()) {
      f(Error() << ErrorKind::FileDoesNotExist);
      return;
    }

    for (auto &[key, value] : mCache) {
      ofs << key << "\n" << value << "\n";
    }
    ofs.close();

    f(std::nullopt);
  }

  void load(std::function<void(std::optional<Error>)> f)
  {
    load(mLocalFile, f);
  }

  void load(Path path, std::function<void(std::optional<Error>)> f)
  {
    std::ifstream file(path);
    if (!file.is_open()) {
      f(Error() << ErrorKind::FileDoesNotExist);
      return;
    }
    std::stringstream buffer;
    buffer << file.rdbuf();
    auto parsedDataOrError = parseData(buffer.str());

    if (std::holds_alternative<Error>(parsedDataOrError)) {
      f(std::get<Error>(parsedDataOrError));
    }
    else {
      auto &newData = std::get<std::unordered_map<std::string, std::string>>(
          parsedDataOrError);
      mCache.insert(newData.begin(), newData.end());
    }
    f(std::nullopt);
  }

  std::unordered_map<std::string, std::string> &cache() { return mCache; }

private:
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
      return Error() << ErrorKind::CorruptPersistenceFile;
    }
    return parsed;
  }

  std::unordered_map<std::string, std::string> mCache;
  Path                                         mLocalFile;
};

} // namespace PB
