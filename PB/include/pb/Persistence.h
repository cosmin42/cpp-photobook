#pragma once

#include <fstream>
#include <functional>
#include <memory>
#include <optional>
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

  void write(std::string path,
             std::function<void(std::optional<Error>)> f)
  {
    write(Path(path), f);
  }

  void write(Path path, std::function<void(std::optional<Error>)> f)
  {
    std::ofstream ofs(path.string());

    if (!ofs.is_open())
    {
      f(Error() << ErrorKind::FileDoesNotExist);
      return;
    }

    for (auto &[key, value] : mCache) {
      ofs << key << "\n" << value << "\n";
    }
    ofs.close();

    f(std::nullopt);
  }

  std::unordered_map<std::string, std::string> &cache() { return mCache; }

private:
  std::unordered_map<std::string, std::string> mCache;
  Path                                         mLocalFile;
};

} // namespace PB
