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

template <typename PersistenceType> class Persistence final {
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

  void load(std::function<void(std::optional<Error>)> f)
  {
    mPersistence.setObserver([f{f}, this](std::optional<Error> out) {
      if (!out) {
        mCache.clear();
        auto &cacheRef = mPersistence.data();
        mCache.insert(cacheRef.begin(), cacheRef.end());
        cacheRef.clear();
      }
      f(out);
    });

    mPersistence.load();
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

} // namespace PB
